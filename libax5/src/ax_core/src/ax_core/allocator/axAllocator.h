//
//  Memory.h
//  ax_core
//
//  Created by Jason on 2015-03-15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once
#include <ax_core/base/ax_base.h>
#include "../pointer/axPtr.h"
#include "../container/axDList.h"
#include "../thread/axSpinLock.h"
#include "../thread/axOptionalLock.h"
#include "../thread/axMutex.h"

#define AX_USE_ALLOCATOR 1

#if AX_COMPILER_VC
	#define AX_ALLOCATOR_OVERRIDE_NEW 0
#endif

const axInt axAllocatorBlockHeaderAlignment = 16;

class axAllocatorError : public axError {};
class axAllocatorError_Undefined							: public axAllocatorError {};
class axAllocatorError_AlignmentIsNotPow2					: public axAllocatorError {};
class axAllocatorError_BlockHeaderIsNotMultipleOfAlignment	: public axAllocatorError {};
class axAllocatorError_NoAllocatorSpecificed				: public axAllocatorError {};

class axAllocator;
class axAllocatorRequest;
class axAllocatorChunkBase;
class axAllocatorBlockHeader;

axAllocator*	ax_root_allocator();

axAllocator*	ax_default_allocator();
axAllocator*	ax_fallback_allocator();

axAllocator*	ax_default_array_allocator();
axAllocator*	ax_default_string_allocator();

axAllocator*	ax_default_list_allocator();
axAllocator*	ax_default_dict_allocator();

axAllocator*	ax_default_ui_allocator();

const axInt axAllocator_kDefaultAlignment = ax_sizeof(void*) * 2;
extern axInt axAllocator_newNonTriviallyDestructibleArrayOffset;

class alignas(axAllocatorBlockHeaderAlignment) axAllocatorBlockHeader {
public:
	axAllocatorBlockHeader(axAllocatorChunkBase* chunk_, axInt sizeInBytes_) : chunk(chunk_), sizeInBytes(sizeInBytes_) {}

	void dealloc();
	axAllocatorChunkBase* chunk;
	axInt64 sizeInBytes;
};

class alignas(axAllocatorBlockHeaderAlignment) axAllocatorBlockFooter {
public:
	axAllocatorBlockFooter(axAllocator* allocator) : _signature(getValidSignature()), _allocator(allocator) {}
	AX_INLINE const void*	getValidSignature() { return (void*)(((intptr_t)this << 16) | 0xF035); }
	AX_INLINE	void		checkSignature()	{ AX_ASSERT(_signature == getValidSignature()); }
	const void*		_signature;
	axAllocator*	_allocator;
};

class axAllocatorChunkBase {
public:
	virtual ~axAllocatorChunkBase() {}

	using BlockHeader = axAllocatorBlockHeader;
	using BlockFooter = axAllocatorBlockFooter;
	using Request = axAllocatorRequest;

	axAllocatorChunkBase(axAllocator* allocator_) : _signature(getValidSignature()), _allocator(allocator_) {}

	void dealloc(BlockHeader* hdr);

	virtual void onDealloc(axInt actualSize, BlockHeader* hdr) = 0;

	AX_INLINE const void*	getValidSignature() { return (void*)(((intptr_t)this << 16) | 0xC035); }
	AX_INLINE	void		checkSignature()	{ AX_ASSERT(_signature == getValidSignature()); }

	const void*		_signature;
	axAllocator*	_allocator;
};

class axAllocatorRequest {
public:
	static const axInt kDefaultAligenment = axAllocator_kDefaultAlignment;

	axAllocatorRequest(	const axSourceLoc& loc_,
						axAllocator* allocator_,
						axInt sizeInBytes_,
						axInt alignment_)
		: loc(loc_)
		, allocator(allocator_)
		, sizeInBytes(sizeInBytes_)
		, alignment(alignment_)
	{}

	axSourceLoc	 loc;
	axAllocator* allocator = nullptr;
	axInt sizeInBytes  = 0;
	axInt alignment = 0;
};

AX_INLINE
std::ostream& operator << (std::ostream& s, const axAllocatorRequest& v) {
	 s << "{\"bytes\": " << v.sizeInBytes << ", "  << v.loc << "\"}";
	 return s;
}

class axAllocatorTrackingChunkPool;

class axAllocator : public axDListNode<axAllocator> {
	using This = axAllocator;
public:
	using ChunkBase = axAllocatorChunkBase;
	using BlockHeader = axAllocatorBlockHeader;
	using BlockFooter = axAllocatorBlockFooter;
	
	static const axInt kDefaultAligenment = axAllocator_kDefaultAlignment;
	using Request = axAllocatorRequest;

	class Result {
	public:
		Result(ChunkBase* chunk_, void* block_) : chunk(chunk_), block(block_) {}
		ChunkBase*	chunk;
		void*		block;
	};

	axAllocator(axAllocator* parent);
	virtual ~axAllocator();

	void*	alloc(const Request& req);

	const axChar* name	() const { return _name; }
	void setName(const axChar* name) { _name = name; }

	axInt allocatedBytes() const { return _allocatedBytes; }
	axInt allocatedCount() const { return _allocatedCount; }

	bool checkMemoryLeak();

	void logInfo();
	void logInfoFile(const char* filename);
	void getInfo(std::ostream& s);

	class TrackingChunk : public ChunkBase, public axDListNode<TrackingChunk> {
		using This = TrackingChunk;
		using Base = ChunkBase;
	public:
		TrackingChunk(ChunkBase* originalChunk_, const Request& req_);
		virtual void onDealloc(axInt actualSize, BlockHeader* hdr) override;

		ChunkBase*		originalChunk;
		Request			req;
	};

	typedef axDList<axAllocator, ax_no_delete> Children;

	AX_INLINE	const void*	getValidSignature() { return (void*)(((intptr_t)this << 16) | 0xA035); }
	AX_INLINE	void		checkSignature()	{ AX_ASSERT(_signature == getValidSignature()); }

	void setUseLock(bool b)			{ AX_ASSERT_MSG(_allocatedCount == 0, "don't change it after any allocation"); _lock.setUseLock(b); }
	bool useLock() const			{ return _lock.useLock(); }

	void setTracking(bool b)		{ AX_ASSERT_MSG(_allocatedCount == 0, "don't change it after any allocation"); _tracking = b; }
	bool tracking() const			{ return _tracking; }

	void setTailStampCheck(bool b)	{ AX_ASSERT_MSG(_allocatedCount == 0, "don't change it after any allocation"); _tailStampCheck = b; }
	bool tailStampCheck()			{ return _tailStampCheck; }

	void setFillMemoryWhenAlloc		(bool b) { _fillMemoryWhenAlloc = b; }
	void setFillMemoryWhenDealloc	(bool b) { _fillMemoryWhenDealloc = b; }

	bool dillMemoryWhenAlloc		() const { return _fillMemoryWhenAlloc; }
	bool dillMemoryWhenDealloc		() const { return _fillMemoryWhenDealloc; }

	virtual Result	onAlloc		(axInt actualSize, const Request& req) = 0;
	virtual void	onGetInfo	(std::ostream& s, axInt indent);

friend class axAllocatorChunkBase;
protected:
	using Lock = axOptionalLock<axInternalMutex>;
	Lock _lock;

	using TrackingChunkList = axDList<TrackingChunk, ax_no_delete>;
	TrackingChunkList	_trackingChunkList;
	
	AX_INLINE void incCounter( axInt numBytes ) {
		_allocatedCount++;
		_allocatedBytes += numBytes;
	}

	AX_INLINE void decCounter( axInt numBytes ) {
		_allocatedCount--;
		_allocatedBytes -= numBytes;
	}


	const void*		_signature;

	void doNewline(std::ostream& s, axInt indent);

	AX_INLINE axInt actualBlockSize(axInt n) {
		axInt o = n;
		o += ax_sizeof(BlockHeader);
		if (_tailStampCheck) {
			o += ax_sizeof(BlockFooter);
		}
		return o;
	}


	axAllocator*	_parent = nullptr;
	Children		_children;
	axAllocatorTrackingChunkPool* _trackingPool = nullptr;
	const axChar*	_name;
	axInt			_allocatedCount = 0;
	axInt			_allocatedBytes = 0;

	bool			_isRoot   : 1;
	bool			_tracking : 1;
	bool			_tailStampCheck : 1;
	bool			_fillMemoryWhenAlloc : 1;
	bool			_fillMemoryWhenDealloc : 1;
};

//--------------------------

AX_INLINE
axAllocator::BlockHeader* axAllocator_getBlockHeader(void* p) {
	auto* hdr = reinterpret_cast<axAllocator::BlockHeader*>(p) - 1;
	hdr->chunk->checkSignature();
	return hdr;
}

AX_INLINE void axAllocatorBlockHeader::dealloc() { chunk->dealloc(this); }

//===================================
void* ax_operator_new(axInt numBytes, const axSourceLoc& loc, axAllocator* allocator, axInt alignment);
void  ax_system_free(void* p);
void* ax_system_malloc(axInt size, axInt alignment);

//--------------------------
#if AX_USE_ALLOCATOR
	#define ax_malloc_impl(ALLOCATOR, T, N, ALIGNMENT) reinterpret_cast<T*>(ax_operator_new(ax_sizeof(T) * (N), AX_LOC, ALLOCATOR, ALIGNMENT))

	template<class T> AX_INLINE
	void ax_free(T* p) { 
		if (!p) {
			AX_ASSERT(false);
			return;
		}
		auto* hdr = axAllocator_getBlockHeader(p);
		hdr->dealloc(); 
	}

	template<class T> AX_INLINE void ax_delete(T* p) {
		if (!p) return;
		auto* hdr = axAllocator_getBlockHeader(p);
		if (hdr->sizeInBytes < ax_sizeof(T)) {
			AX_ASSERT(false);
		}
		ax_call_destructor(p);
		hdr->dealloc();
	}

	template<class T> AX_INLINE void ax_delete_array(T* p) {
		if (!p) return;

		char* c = reinterpret_cast<char*>(p);
		AX_IF_CONSTEXPR (!std::is_trivially_destructible<T>::value) {
			c -= axAllocator_newNonTriviallyDestructibleArrayOffset;
		}
		auto* hdr = axAllocator_getBlockHeader(c);

		if (hdr->sizeInBytes < ax_sizeof(T)) {
			AX_ASSERT(false);
		}

		{	//call destructor
			axInt numObject = hdr->sizeInBytes / ax_sizeof(T);
			auto* it = p;
			auto* end = p + numObject;
			for ( ; it < end; it++ ) {
				it->~T();
			}
		}
		hdr->dealloc();
	}

#else
	#define ax_new_impl(allocator) new
	#define ax_malloc_impl(allocator, T, N, alignment) reinterpret_cast<T*>(::ax_system_malloc(sizeof(T) * (N), alignment))

	template<class T> AX_INLINE void ax_free(T* p) { ::ax_system_free(p); }
	template<class T> AX_INLINE void ax_delete(T* p) { delete p; }
	template<class T> AX_INLINE void ax_delete_array(T* p) { delete[] p; }

#endif

template<class T> AX_INLINE void ax_delete_set_null(T* &p) { ax_delete<T>(p); p = nullptr; }

//--------------------------------------

#if AX_ALLOCATOR_OVERRIDE_NEW
void* operator new(size_t numBytes);
void  operator delete(void* p) noexcept;
#endif

AX_INLINE void* operator new  (size_t numBytes, const axSourceLoc& loc, axAllocator* allocator) { return ax_operator_new(axInt_from_size_t(numBytes), loc, allocator, 0); }
AX_INLINE void* operator new[](size_t numBytes, const axSourceLoc& loc, axAllocator* allocator) { return ax_operator_new(axInt_from_size_t(numBytes), loc, allocator, 0); }

// get called, if constructor throw exception during 'new'
AX_INLINE void operator delete  (void* p, const axSourceLoc&, axAllocator*) { ax_free(p); }
AX_INLINE void operator delete[](void* p, const axSourceLoc&, axAllocator*) { ax_free(p); }

#define ax_new						new(AX_LOC, ax_scope_allocator())
#define ax_new_(ALLOCATOR)			new(AX_LOC, ALLOCATOR)

#define ax_malloc(T,N,ALIGNMENT)	(ax_malloc_impl(ax_scope_allocator(), T, N, ALIGNMENT))

AX_INLINE axAllocator* ax_scope_allocator() { return nullptr; }
