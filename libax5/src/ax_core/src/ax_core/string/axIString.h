#pragma once
#include "axFormatParam.h"
#include "../reflection/axReflection.h"

#define axString_kEnableSmallStringOptimization 1

template<class T> class axIStringT;

template< class T >
class axIStringT : public axNonCopyable {
	AX_RTTI_INTERNAL_CLASS(axIStringT<T>)
public:

	using CharType = T;
	using View     = axStrViewT<T>;
	using MutView  = axStrMutViewT<T>;
	using FormatParam = axFormatParam;

	static const bool _kEnableSmallStringOptimization = axString_kEnableSmallStringOptimization;

	virtual ~axIStringT() = default;

	MutView	view() 			{ return MutView(_storage.data(), _storage.size()); }
	View	view() const	{ return    View(_storage.data(), _storage.size()); }

	operator  MutView() 	{ return view(); }
	operator  View() const	{ return view(); }

	axByteSpan	byteSpan() const { return view().byteSpan(); }

	AX_INLINE			axInt	size		() const				{ return _storage.size(); }
	AX_INLINE			axInt	sizeInBytes	() const				{ return _storage.size() * ax_sizeof(T); }

	AX_INLINE			bool	inBound		(axInt i) const			{ return i >= 0 && i < _storage.size(); }

	AX_INLINE	explicit operator bool() const						{ return _storage.size() != 0; }

	AX_INLINE			T&		operator[]	(axInt i)				{ return at(i); }
	AX_INLINE	const	T&		operator[]	(axInt i) const			{ return at(i); }

	AX_INLINE			T&		at			(axInt i)				{ _checkBound(i); return _storage.data()[i]; }
	AX_INLINE	const	T&		at			(axInt i) const			{ _checkBound(i); return _storage.data()[i]; }

	AX_INLINE 			T&		back		(axInt i = 0)			{ return at( _storage.size()-i-1 ); }
	AX_INLINE 	const	T&		back		(axInt i = 0) const		{ return at( _storage.size()-i-1 ); }

	AX_INLINE 			T&		unsafe_at	(axInt i)				{ _debug_checkBound(i); return _storage.data()[i]; }
	AX_INLINE 	const	T&		unsafe_at	(axInt i) const			{ _debug_checkBound(i); return _storage.data()[i]; }

	AX_INLINE 			T&		unsafe_back	(axInt i)				{ return unsafe_at( _storage.size()-i-1 ); }
	AX_INLINE 	const	T&		unsafe_back	(axInt i)  const		{ return unsafe_at( _storage.size()-i-1 ); }

	AX_INLINE 			T*		data		()						{ return _storage.data(); }
	AX_INLINE	const 	T*		data		() const				{ return _storage.data(); }

	AX_INLINE	const   T*		c_str		() const				{ return _storage.size() ? _storage.data() : axCStr::kEmpty<T>(); }

	//------- View wrapper ---------------
	AX_INLINE			MutView	slice		(axInt offset, axInt size)			{ return view().slice(offset, size); }
	AX_INLINE			View	slice		(axInt offset, axInt size) const	{ return view().slice(offset, size); }

	AX_INLINE			MutView	sliceFrom	(axInt offset) 						{ return view().sliceFrom(offset); }
	AX_INLINE			View	sliceFrom	(axInt offset) const				{ return view().sliceFrom(offset); }

	AX_INLINE			MutView	sliceBack	(axInt offset)						{ return view().sliceBack(offset); }
	AX_INLINE			View	sliceBack	(axInt offset) const				{ return view().sliceBack(offset); }

	AX_INLINE			MutView	sliceLess	(axInt n)							{ return view().sliceLess(n); }	
	AX_INLINE			View	sliceLess	(axInt n) const						{ return view().sliceLess(n); }

	AX_INLINE			MutView	extractFromPrefix(View prefix, bool ignoreCase = false)			{ return view().extractFromPrefix(prefix, ignoreCase); }
	AX_INLINE			View	extractFromPrefix(View prefix, bool ignoreCase = false) const	{ return view().extractFromPrefix(prefix, ignoreCase); }
	//----------------------------------
	AX_INLINE			bool	operator==	(View r) const	{ return equals(r); }
	AX_INLINE			bool	operator!=	(View r) const	{ return !equals(r); }
	AX_INLINE			bool	operator<	(View r) const	{ return compare(r) <  0; }
	AX_INLINE			bool	operator<=	(View r) const	{ return compare(r) <= 0; }
	AX_INLINE			bool	operator>	(View r) const	{ return compare(r) >  0; }
	AX_INLINE			bool	operator>=	(View r) const	{ return compare(r) >= 0; }

	AX_INLINE			axInt	compare		(View r,  bool ignoreCase = false ) const { return view().compare	(r,  ignoreCase); }
	AX_INLINE			bool	equals		(View r,  bool ignoreCase = false ) const { return view().equals	(r,  ignoreCase); }

	AX_INLINE			bool	startsWith	(View r,  bool ignoreCase = false ) const { return view().startsWith(r,  ignoreCase); }
	AX_INLINE			bool	endsWith	(View r,  bool ignoreCase = false ) const { return view().endsWith	(r,  ignoreCase); }

	AX_INLINE			bool	matchWildcard(View wildcard, bool ignoreCase) const { return view().matchWildcard(wildcard, ignoreCase); }

	//-----------------------------------
							axInt	find				(View str,  bool ignoreCase = false) const	{ return view().find			(str,  ignoreCase); }
							axInt	findBack			(View str,  bool ignoreCase = false) const	{ return view().findBack		(str,  ignoreCase); }

							axInt	findChar			(T    ch,   bool ignoreCase = false) const	{ return view().findChar		(ch,   ignoreCase); }
							axInt	findCharBack		(T    ch,   bool ignoreCase = false) const	{ return view().findCharBack	(ch,   ignoreCase); }

							axInt	findChars			(View list, bool ignoreCase = false) const	{ return view().findChars		(list, ignoreCase); }
							axInt	findCharsBack		(View list, bool ignoreCase = false) const	{ return view().findCharsBack	(list, ignoreCase); }

		axPair<MutView, MutView>	split				(View str,  bool ignoreCase = false)		{ return view().split			(str,  ignoreCase); }
		axPair<MutView, MutView>	splitBack			(View str,  bool ignoreCase = false)		{ return view().splitBack		(str,  ignoreCase); }

		axPair<MutView, MutView>	splitByChar			(T    ch,   bool ignoreCase = false)		{ return view().splitByChar		(ch,   ignoreCase); }
		axPair<MutView, MutView>	splitByCharBack		(T    ch,   bool ignoreCase = false)		{ return view().splitByCharBack	(ch,   ignoreCase); }

		axPair<MutView, MutView>	splitByChars		(View list, bool ignoreCase = false)		{ return view().splitByChars	(list, ignoreCase); }
		axPair<MutView, MutView>	splitByCharsBack	(View list, bool ignoreCase = false)		{ return view().splitByCharsBack(list, ignoreCase); }

		axPair<MutView, MutView>	splitByIndex		(axInt index) { return view().splitByIndex(index); }

							void	getTokens			(axIArray<MutView> &outList, T separator, bool ignoreCase = false) {
								return view().getTokens(outList, separator, ignoreCase);
							}

							template<class V>
							bool	tryParse			(V& outValue) const		{ return view().tryParse(outValue); }

    //---------------
    template<class R> using Enumerator_    = axArrayEnumerator<R>;
    template<class R> using RevEnumerator_ = axArrayRevEnumerator<R>;

	using Enumerator  = Enumerator_<T>;
	using CEnumerator = Enumerator_<const T>;
	using RevEnumerator  = RevEnumerator_<T>;
	using CRevEnumerator = RevEnumerator_<const T>;
	using Iterator			= T*;
	using CIterator			= const T*;

    Enumerator		each	()			{ return Enumerator(  _storage.data(), _storage.data() + _storage.size() ); }
    CEnumerator		each	() const	{ return CEnumerator( _storage.data(), _storage.data() + _storage.size() ); }

    RevEnumerator	revEach	()			{ return RevEnumerator(  _storage.data(), _storage.data() + _storage.size() ); }
    CRevEnumerator	revEach	() const	{ return CRevEnumerator( _storage.data(), _storage.data() + _storage.size() ); }

	Iterator		begin	()			{ return _storage.data(); }
    CIterator		begin	() const	{ return _storage.data(); }
	Iterator		end		()			{ return _storage.data() + _storage.size(); }
    CIterator		end		() const	{ return _storage.data() + _storage.size(); }
    //---------------

    //-------- mutable functions ---------------
				void	setToDefaultValue() { clear(); }
	AX_INLINE	void	clear			() { resize(0); }
				void	clearAndFree	();

	AX_INLINE	axInt	capacity		() const { return _storage.capacity(); }

	AX_INLINE	void	resize			(axInt new_size);
	AX_INLINE	void	incSize			(axInt n)		{ if (n < 0) { AX_ASSERT(false); return; } resize(_storage.size() + n); }
	AX_INLINE	void	decSize			(axInt n)		{ if (n < 0) { AX_ASSERT(false); return; } resize(_storage.size() - n); }

	AX_INLINE	void	reserve			(axInt n);
	AX_INLINE	void	reserveMore		(axInt n)		{ if (n < 0) { AX_ASSERT(false); return; } reserve(_storage.size() + n); }
	AX_INLINE	void	reserveLess		(axInt n)		{ if (n < 0) { AX_ASSERT(false); return; } reserve(_storage.size() - n); }

					void	move			(axIStringT & rhs);

					void	appendHex		(axByteSpan src, bool upperCase = true, T separator = 0);

	template<class... ARGS>
	AX_INLINE	This&	set				(ARGS&&... args) &						{ clear(); append(ax_forward(args)...); return *this; }
	AX_INLINE	This&	set_c_str		(const T* sz) &							{ clear(); append_c_str(sz); return *this; }
	AX_INLINE	This&	setRepeat		(View rhs, axInt repeat) &				{ clear(); appendRepeat( rhs, repeat ); return *this; }

	template<class... ARGS>
	AX_INLINE	This&	append			(const ARGS&... args) &;
	AX_INLINE	This&	append_c_str	(const T* sz) &							{ _append(axStrView_c_str(sz)); return *this; }
	AX_INLINE 	This&	appendRepeat	(View rhs, axInt repeat) &				{ _append( rhs, repeat ); return *this; }

#define AX_MACRO_OP(R) \
					This&	appendUtfRepeat	(axStrViewT<R> rhs, axInt repeat) &; \
	AX_INLINE	This&	appendUtf		(axStrViewT<R> rhs ) & 					{ appendUtfRepeat( rhs, 1 ); return *this; } \
	\
	AX_INLINE	This&	appendUtf_c_str	(const R*   sz) &						{ appendUtf(axStrView_c_str(sz)); return *this; } \
	AX_INLINE	This&	appendUtf		(const R &  rhs) &						{ appendUtf(axStrView_char(rhs) ); return *this; } \
	AX_INLINE	This&	appendUtfRepeat	(const R &  rhs, axInt repeat) &		{ appendUtfRepeat(axStrView_char(rhs), repeat); return *this; } \
	\
	AX_INLINE	This&	setUtf_c_str	(const R*   sz) &						{ clear(); appendUtf(axStrView_c_str(sz)); return *this; } \
	AX_INLINE	This&	setUtf			(const R &  rhs) &						{ clear(); appendUtf( rhs ); return *this; } \
	AX_INLINE	This&	setUtfRepeat	(const R &  rhs, axInt repeat) &		{ clear(); appendUtfRepeat( rhs, repeat ); return *this; } \
	\
	AX_INLINE	This&	setUtf			(axStrViewT<R> rhs) & 					{ clear(); appendUtf( rhs ); return *this; } \
	AX_INLINE	This&	setUtfRepeat	(axStrViewT<R> rhs, axInt repeat) &		{ clear(); appendUtfRepeat( rhs, repeat ); return *this; } \
//------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef  AX_MACRO_OP

	template<class V> axIStringT<T>&	convert			(const V& v) &; // implement in axFormat.h
	template<class V> axIStringT<T>&	appendConvert	(const V& v) &; // implement in axFormat.h

					void	insert			(axInt pos, View text);
					void	removeChars		(axInt pos, axInt count);

	AX_INLINE	void	operator=		( View rhs )				{ set(rhs); }
	AX_INLINE	void	operator=		( const axIStringT&  rhs )	{ set(rhs); }
	AX_INLINE	void	operator=		(       axIStringT&& rhs )	{ move(rhs); }

	AX_INLINE	void	operator+=		( T ch )					{ append(ch); }
	AX_INLINE	void	operator+=		( View rhs )				{ append(rhs); }

	template<class FMT>					void	format		(const FMT& fmtStr)							{ clear(); appendFormat(fmtStr); }
	template<class FMT, class... Args>	void	format		(const FMT& fmtStr, const Args&... args)	{ clear(); appendFormat(fmtStr, args...); }
	template<class FMT>					void	appendFormat(const FMT& fmtStr);
	template<class FMT, class... Args>	void	appendFormat(const FMT& fmtStr, const Args&... args);

		void	appendFormatWithParamList(axStrView8  fmtStr, const axSpan<FormatParam>& paramList);
		void	appendFormatWithParamList(axStrView16 fmtStr, const axSpan<FormatParam>& paramList);
		void	appendFormatWithParamList(axStrView32 fmtStr, const axSpan<FormatParam>& paramList);
		void	appendFormatWithParamList(axStrViewW  fmtStr, const axSpan<FormatParam>& paramList);

	void onFormat(axFormat& f) const { view().onFormat(f); }

	axHash onGetHash() const { return view().onGetHash(); }

	void replaceChars(T from, T to);

	axInt replaceAll(View from, View to, bool ignoreCase);
	
	void setAllocator(axAllocator* allocator) { _allocator = allocator; }

friend class axFormat;
protected:
	axIStringT() = default;

	axAllocator* useAllocator() { return _allocator ? _allocator : ax_default_string_allocator(); }

	template<class FormatChar>
	void	_appendFormatWithParamList (axStrMutViewT<const FormatChar> fmt, const axSpan<FormatParam>& paramList );

	AX_INLINE void	_checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		AX_UNUSED(i);
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

	template< class R >
	void	_appendUtf	(axStrViewT<R> v, axInt repeat);
	void	_append		(View view, axInt repeat = 1);

	virtual bool isDataMovable() const = 0;

	virtual	axMutSpan<T>	onMalloc	(axInt reqSize) = 0;
	virtual	void			onFree		(T* p) = 0;

			void	_reserve(axInt newSize);

	struct NormalStorage {
		axInt	_capacity;
		axInt	size;
		T*		data;

		AX_STATIC_ASSERT(sizeof(axInt) == sizeof(axUInt));
		static constexpr axInt kCapMask = static_cast<axInt>(axUInt(1) << (sizeof(axInt) * 8 - 1));
		axInt	capacity() const {
			#if axString_kEnableSmallStringOptimization
				return axByteOrder::BigEndian::toHost(_capacity) & ~kCapMask;
			#else
				return _capacity;
			#endif
		}
		void	setCapacity(axInt v) {
			#if axString_kEnableSmallStringOptimization
				_capacity = axByteOrder::BigEndian::fromHost(v | kCapMask);
			#else
				_capacity = v;
			#endif
		}
	};

	struct SmallStorage {
		axInt8	size;
		static constexpr axInt capacity = sizeof(NormalStorage) - alignof(T) - 1; // -1 for Null terminator
		AX_STATIC_ASSERT(capacity > 0 && capacity < 0x80);
		T		data[capacity + 1]; // +1 for Null terminator
	};

	union Storage {
	public:
		SmallStorage	_small;
		NormalStorage	_normal;

		Storage() { reset(); }

		void reset() {
			#if axString_kEnableSmallStringOptimization
				_small.size = 0;
			#else
				_normal._capacity = 0;
				_normal.size = 0;
				_normal.data = nullptr;
			#endif
		}

		~Storage() {
			#if axString_kEnableSmallStringOptimization
				AX_ASSERT_MSG(isSmall() || _normal.data == nullptr, "axIString data is not freed");
			#else
				AX_ASSERT_MSG(_normal.data == nullptr, "axIString data is not freed");
			#endif
		}

		constexpr bool isSmall() const	{
			#if axString_kEnableSmallStringOptimization
				return _small.size >= 0;
			#else
				return false;
			#endif
		}

		axInt	capacity() const	{ 
			#if axString_kEnableSmallStringOptimization
				return isSmall() ? _small.capacity	: _normal.capacity();
			#else
				return _normal.capacity();
			#endif
		}

	const	T*	data	() const	{ return isSmall() ? _small.data : _normal.data; }
			T*	data	()			{ return isSmall() ? _small.data : _normal.data; }
		axInt	size	() const	{ return isSmall() ? _small.size : _normal.size; }

		void	setSize(axInt v)	{
			if (isSmall()) {
				AX_ASSERT(v < 0x80);
				_small.size = static_cast<axInt8>(v);
			} else {
				_normal.size = v;
			}
		}

		void setData(T* data, axInt cap) {
//			printf("_storage.setData %lld\n", cap);
			auto oldSize = size();
			_normal.size = oldSize;
			_normal.data = data;
			_normal.setCapacity(cap);
		}
	};

	Storage			_storage;
	axAllocator*	_allocator = nullptr;
};

namespace axTypeTraits {
	template<class T>	struct spanType<    axIStringT<T> > { using Type = axStrViewT<T>; };
	template<class T>	struct mutSpanType< axIStringT<T> > { using Type = axStrMutViewT<T>; };
}

template< class T > inline
std::ostream& operator << ( std::ostream & s, const axIStringT<T> & v ) { return s << v.c_str(); }

//---------------------

template< class T > inline
void	axIStringT<T>::resize	( axInt newSize ) {
	if (newSize < 0) throw axError_Undefined(AX_LOC);
	if (newSize == size()) return;

	auto oldSize = size();
	if (newSize > oldSize) {
		reserve( newSize );
	}

	_storage.setSize(newSize);
	if (_storage.capacity() > 0) {
		_storage.data()[newSize] = 0;
	}
}

template< class T > inline
void	axIStringT<T>::reserve(axInt newSize) {
	if (newSize > _storage.capacity()) {
		_reserve(newSize);
	}
}

template< class T > inline
void	axIStringT<T>::move(axIStringT<T> & rhs) {
	if (this == &rhs) return;

	if (rhs.isDataMovable() && !_storage.isSmall() && !rhs._storage.isSmall()) {
		clearAndFree();
		_storage.setData(rhs.data(), rhs.capacity());
		_storage.setSize(rhs.size());
		rhs._storage.reset();
		return;
	}

	// unable to move, so copy
	set(rhs);
	rhs.clear();
}

template<class T>
template<class... ARGS> inline
axIStringT<T>& axIStringT<T>::append(const ARGS&... args) & {
	View views[] = {View(args)...};
	axInt totalSize = size();
	for (auto& v : views) {
		totalSize += v.size();
	}
	reserve(totalSize);

	for (auto& v : views) {
		_append(v);
	}
	return *this;
}

template<class T>
template<class FMT> AX_INLINE
void axIStringT<T>::appendFormat(const FMT& fmt) {
	appendFormatWithParamList(fmt, axSpan<FormatParam>());
}

template<class T>
template<class FMT, class ...Args> AX_INLINE
void axIStringT<T>::appendFormat(const FMT& fmt, const Args & ...args) {
	FormatParam tmp[] {args...};
	appendFormatWithParamList(fmt, axSpan_make(tmp));
}
