#include <ax_core/allocator/axAllocator.h>
#include <ax_core/allocator/axSystemAllocator.h>
#include <ax_core/log/axLog.h>
#include <ax_core/other/axGlobalSingleton.h>

axInternalSpinLock axAllocator_children_lock;

axInt axAllocator_newNonTriviallyDestructibleArrayOffset = 0;

void* ax_system_malloc(axInt size, axInt alignment) {
	#if AX_OS_WINDOWS
		return _aligned_malloc(size, alignment);
	#else
		void* p = nullptr;
		int ret = ::posix_memalign(&p, axInt_to_size_t(alignment), axInt_to_size_t(size));
		if (ret) return nullptr;
		return p;
	#endif
}

void ax_system_free(void* p) {
	#if AX_OS_WINDOWS
		::_aligned_free(p);
	#else
		::free(p);
	#endif
}

void* ax_operator_new(axInt numBytes, const axSourceLoc& loc, axAllocator* allocator, axInt alignment) {
	if (!allocator) {
		allocator = ax_fallback_allocator();
	}

	if (alignment < axAllocator_kDefaultAlignment) {
		alignment = axAllocator_kDefaultAlignment;
	}
	
	if (numBytes >= axNumLimit<axInt>::max()) {
		throw axError_Undefined(AX_LOC);
	}
	axAllocatorRequest req(loc, allocator, numBytes, alignment);
	return allocator->alloc(req);
}

//------- Tracking Chunk -----------------

class axAllocatorTrackingChunkPool : public axNonCopyable {
	using This = axAllocatorTrackingChunkPool;
public:
	axAllocatorTrackingChunkPool() {
		//_lock.setUseLock(false);
		//AX_DUMP_VAR(ax_sizeof(axAllocatorChunkBase));
		//AX_DUMP_VAR(ax_sizeof(axDListNode<Chunk>));
		//AX_DUMP_VAR(ax_sizeof(Chunk));
		//AX_DUMP_VAR(ax_sizeof(Page));
	}

	~axAllocatorTrackingChunkPool() { _dtor(); }

	using Chunk = axAllocator::TrackingChunk;

	static This* s_instance() {
		static axGlobalSingleton<This> s;
		return s.ptr();
	}

	struct Page : public axDListNode<Page> {
		static const axInt kSize = 2048;
		char	buffer[sizeof(Chunk) * kSize];
		axInt	used = 0;
		AX_INLINE axInt	freeCount() const { return kSize - used; }
	};

	AX_INLINE void* get() {
		auto lock = _lock.scopedLock();
		auto p = _freeList.popHead();
		if (p) {
			_usedChunkCount++;
			return p.detach();
		}

		auto* page = _pages.tail();
		if (!page || !page->freeCount()) {
			auto* m = ax_system_malloc(ax_sizeof(Page), axAllocator_kDefaultAlignment);
			page = new(m) Page();
			_pages.append(page);
		}

		auto* chunk = &page->buffer[page->used * ax_sizeof(Chunk)];
		page->used++;
		_usedChunkCount++;
		return chunk;
	}

	AX_INLINE void release(Chunk* p) {
		auto lock = _lock.scopedLock();

		p->originalChunk = nullptr;
		p->req.allocator = nullptr;
		p->req.loc = AX_LOC;

		_usedChunkCount--;
		_freeList.append(p);
	}

private:
	void _dtor() {
		axInt freeListCount = _freeList.size();
		_freeList.removeAll();

		axInt totalInPages = 0;
		while (auto* p = _pages.popTailPtr()) {
			totalInPages += p->used;
			ax_system_free(p);
		}

		AX_ASSERT(_usedChunkCount == 0);
		AX_ASSERT(freeListCount == totalInPages);
	}

	using Lock = axOptionalLock<axInternalSpinLock>;
	Lock	_lock;
	axDList<Page>	_pages;
	axDList<Chunk>	_freeList;
	axInt _usedChunkCount = 0;
};

AX_INLINE axAllocator::TrackingChunk::TrackingChunk(ChunkBase* originalChunk_, const Request& req_)
	: Base(req_.allocator)
	, originalChunk(originalChunk_)
	, req(req_)
{
}

void axAllocator::TrackingChunk::onDealloc(axInt actualSize, BlockHeader* hdr) {
	AX_ASSERT(_allocator == originalChunk->_allocator);
	_allocator->_trackingChunkList.remove(this);
	originalChunk->onDealloc(actualSize, hdr);
	
	ax_call_destructor(this);
	_allocator->_trackingPool->release(this);
}

//-------- axAllocator ----------------
axAllocator::axAllocator(axAllocator* parent)
	: _signature(getValidSignature())
    , _name(AX_SZ("axAllocator"))
    , _allocatedCount(0)
	, _allocatedBytes(0)
    , _isRoot(false)
    , _tracking(true)
	, _tailStampCheck(true)
	, _fillMemoryWhenAlloc(true)
	, _fillMemoryWhenDealloc(true)
{
	// ensure init axAllocatorTrackingChunkPool first
	_trackingPool = axAllocatorTrackingChunkPool::s_instance();

	if (parent) {
		auto lock = axAllocator_children_lock.scopedLock();
		auto* p = parent;
		while (p) {
			if (p == this) {
				AX_ASSERT_MSG(false, "axAllocator parent cycle check");
				return;
			}
			p = p->_parent;
		}
		_parent = parent;
		_parent->_children.append(this);
	}
}

axAllocator::~axAllocator() {
	auto lock = _lock.scopedLock();

	if (_allocatedCount != 0 || _allocatedBytes != 0) {
		logInfo();
		AX_ASSERT_MSG(false, "Memory Leak !!");
	}

	{
		auto clock = axAllocator_children_lock.scopedLock();
		if (_parent) {
			_parent->_children.remove(this);
		}

		if (_children.size()) {
			AX_ASSERT_MSG(false, "destroy allocator contains child allocator");
		}
	}
}

void* axAllocator::alloc(const Request& req) {
	AX_ASSERT(req.allocator == this);

	if (!_parent && !_isRoot ) {
		AX_ASSERT_MSG(false, "parent cannot be null, other than root allocator");
	}

	const axInt hdrSize  = ax_sizeof(BlockHeader);

	if (!ax_is_pow2(req.alignment)) throw axAllocatorError_AlignmentIsNotPow2();
	if (!ax_multiple_of(hdrSize, req.alignment)) throw axAllocatorError_BlockHeaderIsNotMultipleOfAlignment();

	auto lock = _lock.scopedLock();

	axInt actualSize = actualBlockSize(req.sizeInBytes);
	auto res = onAlloc(actualSize, req);
	if (!res.block) return nullptr;

	if (res.chunk == nullptr) {
		AX_ASSERT_MSG(false, "outChunk cannot be null");
		return nullptr;
	}

	auto* hdr = new(res.block) BlockHeader(res.chunk, req.sizeInBytes);
	char* outPtr = reinterpret_cast<char*>(hdr + 1);
	
	if (_tailStampCheck) {
		new(outPtr + req.sizeInBytes) BlockFooter(this);
	}

	if (_fillMemoryWhenAlloc) {
		ax_memset(outPtr, 0x6A, req.sizeInBytes);
	}

	if (_tracking) {
#if TRACY_ENABLE
		TracyAlloc (outPtr, req.sizeInBytes);
#endif
		auto* tcBlock = _trackingPool->get();
		auto* tc = new(tcBlock) TrackingChunk(res.chunk, req);
		_trackingChunkList.append(tc);
		hdr->chunk = tc; // redirect to tracking chunk
	}

	incCounter(req.sizeInBytes);
	return outPtr;
}

void axAllocatorChunkBase::dealloc(BlockHeader* hdr) {
	if (_allocator->tailStampCheck()) {
		auto* p = reinterpret_cast<char*>(hdr + 1) + hdr->sizeInBytes;
		auto* footer = reinterpret_cast<BlockFooter*>(p);
		footer->checkSignature();
		AX_ASSERT(footer->_allocator == _allocator);
	}

	if (_allocator->_tracking) {
#if TRACY_ENABLE
		TracyFree (hdr + 1);
#endif
	}

	if (_allocator->_fillMemoryWhenDealloc) {
		ax_memset(hdr + 1, 0x3F, hdr->sizeInBytes);
	}

	auto lock = _allocator->_lock.scopedLock();
	_allocator->decCounter(hdr->sizeInBytes);

	axInt actualSize = _allocator->actualBlockSize(hdr->sizeInBytes);
	onDealloc(actualSize, hdr);
}

bool axAllocator::checkMemoryLeak() {
	if (_allocatedCount != 0 || _allocatedBytes != 0) {
		AX_ASSERT_MSG(false, "memory leak");
		return false;
	}
	return true;
}

void axAllocator::logInfo() {	
	getInfo(std::cout);
}

void axAllocator::logInfoFile(const char* filename) {
	std::ofstream fs;
	fs.open(filename);
	getInfo(fs);
	fs.close();
}

void axAllocator::getInfo(std::ostream& s) {
	auto clock = axAllocator_children_lock.scopedLock();

	axInt indent = 0;
	doNewline(s, indent); s << "{";
	indent++;
		doNewline(s, indent); s << "\"axAllcoatorInfo\": {";
		indent++;
			doNewline(s, indent); s << "\"BlockHeaderSize\": " << ax_sizeof(BlockHeader) << ",";
			doNewline(s, indent); s << "\"TrackingChunkSize\": " << ax_sizeof(TrackingChunk) << ",";
			doNewline(s, indent); s << "\"Allocators\":[";
			onGetInfo(s, indent+1);
			doNewline(s, indent); s << "]";
		indent--;
		doNewline(s, indent); s << "}";
	indent--;
	doNewline(s, indent); s << "}";
	s << "\n\n";
}

void axAllocator::doNewline(std::ostream & s, axInt indent) {
	s << "\n";
	for (axInt i=0; i<indent; i++) { 
		s << "  ";
	}
}

void axAllocator::onGetInfo(std::ostream& s, axInt indent) {
	{	
		doNewline(s, indent); s << "{";
		indent++;
			auto lock = _lock.scopedLock();
			doNewline(s, indent); s << "\"name\": \"" << name() << "\",";
			doNewline(s, indent); s << "\"bytes\": " << _allocatedBytes << ",";
			doNewline(s, indent); s << "\"numBlocks\": " <<  _allocatedCount << ",";
			doNewline(s, indent); s << "\"blocks\": [";
			indent++;
			axInt i = 0;
			for (auto& tc : _trackingChunkList) {
				if (i > 0) s << ",";
				doNewline(s, indent);
				s << tc.req;
				i++;
			}
		indent--;
		doNewline(s, indent); s << "],";
	}

	doNewline(s, indent); s << "children: [";
	{
		axInt i = 0;
		for (auto& c : _children) {
			if (i > 0) s << ",";
			c.onGetInfo(s, indent + 1);
			i++;
		}
	}
	doNewline(s, indent); s << "]";
	indent--;
	doNewline(s, indent); s << "}";
}

//------------------------------

class axAllocator_TestNewArrayOffset {
public:
	axAllocator_TestNewArrayOffset();

	struct Foo {
		~Foo() {}; // is_trivially_destructible
		char padding[32];
	};

	axInt offset = 0;
	axInt extraSize = 0;

	char buffer[sizeof(Foo)];

	void* _onNew(size_t numBytes) {
		extraSize = static_cast<axInt>(numBytes - sizeof(Foo));
		return buffer;
	}
};

inline void* operator new[](size_t numBytes, const axSourceLoc& loc, axAllocator_TestNewArrayOffset* test) {
	return test->_onNew(numBytes);
}

axAllocator_TestNewArrayOffset::axAllocator_TestNewArrayOffset() {
	Foo* p = new(AX_LOC, this) Foo[1];
	offset = intptr_t(reinterpret_cast<char*>(p) - buffer);
}


class axRootAllocator : public axSystemAllocator {
	typedef axSystemAllocator Base;
public:
	axRootAllocator() : Base(nullptr) { 
		_isRoot = true;
		setName("axRootAllocator");

		axAllocator_TestNewArrayOffset t;
		axAllocator_newNonTriviallyDestructibleArrayOffset = t.offset;
	}
};

axAllocator* ax_root_allocator() {
	static axGlobalSingleton<axRootAllocator> s;
	return s.ptr(); 
}

//---------------------

#define ax_define_default_allocator(FUNC) \
	class FUNC##_Class : public axSystemAllocator { \
		typedef axSystemAllocator Base; \
	public: \
		FUNC##_Class() : Base(ax_root_allocator()) { setName(#FUNC); } \
	}; \
	axAllocator* FUNC() { \
		static axGlobalSingleton<FUNC##_Class> s; \
		return s.ptr(); \
	} \
//--------

ax_define_default_allocator(ax_fallback_allocator           );
ax_define_default_allocator(ax_default_allocator            );

ax_define_default_allocator(ax_default_array_allocator      );
ax_define_default_allocator(ax_default_string_allocator     );

ax_define_default_allocator(ax_default_list_allocator       );
ax_define_default_allocator(ax_default_dict_allocator       );
ax_define_default_allocator(ax_default_ui_allocator  );

#if AX_USE_ALLOCATOR
	#if false //check any usage from default new
		void* operator new(size_t numBytes) {
		//	AX_ASSERT(false, "please using ax_new");
		//	throw axError_AllocatorIsNull();
			return ::malloc(numBytes);
		}
	#endif

	#if AX_ALLOCATOR_OVERRIDE_NEW

	#if AX_COMPILER_VC
	void  operator delete(void* p) noexcept {
		AX_ASSERT_MSG(false, "please use ax_delete");
	}

	void  operator delete[](void* p) noexcept {
		 AX_ASSERT_MSG(false, "please use ax_delete_array");
	}
	#endif

	void* operator new(size_t numBytes) {
		assert(false); // please using ax_new
	//	ax_force_crash();
		return ::malloc(numBytes);
	};
	#endif

#endif
