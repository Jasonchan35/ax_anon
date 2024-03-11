#pragma once
#include "axSpan.h"
#include "axFixedArray.h"

class axType;
class axAllocator;

template<class T> class axIArray;

template<class T>
class axIArray : public axNonCopyable {
	using This = axIArray<T>;
public:
	using MutSpan = axMutSpan<T>;
	using Span    = axSpan<T>;

	MutSpan	mutSpan		()			{ return MutSpan(_data, _size); }
	MutSpan	span		()			{ return MutSpan(_data, _size); }
	Span	span		() const	{ return Span(_data, _size); }
	Span	constSpan	() const	{ return Span(_data, _size); }

	operator  MutSpan	() 			{ return mutSpan(); }
	operator  Span		() const	{ return span(); }

	virtual ~axIArray();
	
	static const axType* s_rtti();

	AX_INLINE	 	  T &	operator[]	( axInt i )							{ return at(i); }
	AX_INLINE	const T &	operator[]	( axInt i ) const					{ return at(i); }

	AX_INLINE	 	  T &	at			( axInt i )							{ _checkBound(i); return _data[i]; }
	AX_INLINE	const T &	at			( axInt i ) const					{ _checkBound(i); return _data[i]; }

	AX_INLINE 		  T &	unsafe_at	( axInt i ) 						{ _debug_checkBound(i); return _data[i]; }
	AX_INLINE 	const T &	unsafe_at	( axInt i ) const					{ _debug_checkBound(i); return _data[i]; }

	AX_INLINE	 	  T *	tryAt		( axInt i )							{ return inBound(i) ? &_data[i] : nullptr; }
	AX_INLINE	const T *	tryAt		( axInt i ) const					{ return inBound(i) ? &_data[i] : nullptr; }

	AX_INLINE	void		removeAt	( axInt i );

	AX_INLINE 		  T &	back		()									{ return at( _size-1 ); }
	AX_INLINE 	const T &	back		() const							{ return at( _size-1 ); }

	AX_INLINE 		  T &	back		( axInt i )							{ return at( _size-i-1 ); }
	AX_INLINE 	const T &	back		( axInt i ) const					{ return at( _size-i-1 ); }

	AX_INLINE 		  T &	unsafe_back	( axInt i )							{ return unsafe_at( _size-i-1 ); }
	AX_INLINE 	const T &	unsafe_back	( axInt i )  const					{ return unsafe_at( _size-i-1 ); }

	AX_INLINE 	void		popBack		()									{ decSize(1); }
	AX_INLINE 	void		popBack		( T& out_value )					{ out_value = ax_move(back()); decSize(1); }

	//------- forward View functions ----------------
	AX_INLINE	MutSpan	slice		( axInt offset, axInt size )			{ return span().slice(offset, size); }
	AX_INLINE	Span	slice		( axInt offset, axInt size ) const		{ return span().slice(offset, size); }

	AX_INLINE	MutSpan	slice		( axIntRange range )					{ return span().slice(range); }
	AX_INLINE	Span	slice		( axIntRange range ) const				{ return span().slice(range); }

	AX_INLINE	MutSpan	sliceFrom	( axInt offset )						{ return span().sliceFrom(offset); }
	AX_INLINE	Span	sliceFrom	( axInt offset ) const					{ return span().sliceFrom(offset); }

	AX_INLINE	MutSpan	sliceBack	( axInt offset )						{ return span().sliceBack(offset); }
	AX_INLINE	Span	sliceBack	( axInt offset ) const					{ return span().sliceBack(offset); }

	AX_INLINE	MutSpan	sliceLess	(axInt n)								{ return span().sliceLess(n); }
	AX_INLINE	Span	sliceLess	(axInt n) const							{ return span().sliceLess(n); }

	AX_INLINE	void	fillValues	( const T& v )							{ span().fillValues(v); }

	//----------------------
	AX_INLINE	axInt	size		() const			{ return _size; }
	AX_INLINE	axInt	sizeInBytes	() const			{ return _size * ax_sizeof(T); }

	AX_INLINE	bool	inBound		( axInt  i ) const	{ return i >= 0 && i < _size; }

		axMutSpan<axByte>	toByteSpan	()				{ return axMutSpan<axByte>(reinterpret_cast<axByte*>(_data), _size * ax_sizeof(T)); }
		axSpan<axByte>		toByteSpan	() const		{ return axSpan<   axByte>(reinterpret_cast<axByte*>(_data), _size * ax_sizeof(T)); }

	AX_INLINE 			T*	data	()				{ return _data; }
	AX_INLINE	const 	T*	data	() const		{ return _data; }

    //---------------
    template<class R> using Enumerator_    = axArrayEnumerator<R>;
    template<class R> using RevEnumerator_ = axArrayRevEnumerator<R>;

	using Enumerator		= Enumerator_<T>;
	using CEnumerator		= Enumerator_<const T>;
	using RevEnumerator		= RevEnumerator_<T>;
	using CRevEnumerator	= RevEnumerator_<const T>;
	using Iterator			= T*;
	using CIterator			= const T*;

    Enumerator	each	()			{ return Enumerator(  _data, _data + _size ); }
    CEnumerator	each	() const	{ return CEnumerator( _data, _data + _size ); }

    RevEnumerator	revEach	()			{ return RevEnumerator(  _data, _data + _size ); }
    CRevEnumerator	revEach	() const	{ return CRevEnumerator( _data, _data + _size ); }

	Iterator	begin	()			{ return _data; }
    CIterator	begin	() const	{ return _data; }
    Iterator	end		()			{ return _data + _size; }
    CIterator	end		() const	{ return _data + _size; }
    //---------------

    //------- mutable functions ---
	AX_INLINE	void	clear			() { resize(0); }
	AX_INLINE	void	clearAndFree	();
	AX_INLINE	void	clearAndReserve	(axInt n) { clear(); reserve(n); }

	AX_INLINE	axInt	capacity	() const						{ return _capacity; }
	AX_INLINE 	void	reserve		( axInt newSize );
	AX_INLINE 	void	incReserve	( axInt n )						{ if (n < 0) { AX_ASSERT(false); return; } reserve(_size + n); }

	template< class... Args >
	AX_INLINE 	void	resize		( axInt newSize, Args&&... newElementConstructArgs );

	template< class... Args >
	AX_INLINE 	void	resizeNoCopy( axInt newSize, Args&&... newElementConstructArgs );

	template< class... Args >
	AX_INLINE 	void	resizeNoInit( axInt newSize) { resize(newSize, axTag::NoInit); }

	template< class... Args >
	AX_INLINE 	void	resizeToCap	( Args&&... newElementConstructArgs ) { resize(capacity(), ax_forward(newElementConstructArgs)...); }

	template< class... Args >
	AX_INLINE	void		incSize		( axInt n, Args&&... newElementConstructArgs ) { 
		if (n < 0) { AX_ASSERT(false); return; } 
		resize(_size + n, ax_forward(newElementConstructArgs)...);
	}

	template< class... Args >
	AX_INLINE	void	decSize		( axInt n ) { if (n < 0) { AX_ASSERT(false); return; } resize(_size - n); }

	template< class... Args >
	AX_INLINE	T&		emplaceBack	( Args&&... args )				{ resize( _size + 1, ax_forward(args)...); return back(); }

	AX_INLINE	void	append		( const T &  v );
	AX_INLINE 	void	append		(       T && v );

	AX_INLINE 	void	appendRange	( Span rhs );
	AX_INLINE	void	appendRange	( This && rhs );

	template<class R>
	AX_INLINE	void	appendRangeByCopy( const R& rhs );

	template<class R>
	AX_INLINE	void	appendRangeByMove( R& rhs );

	AX_INLINE	void	set			( Span rhs )						{ clear(); appendRange( rhs ); }

	AX_INLINE	void	operator=	( Span rhs )						{ set(rhs); }
	AX_INLINE	void	operator=	( This && rhs )						{ move(rhs); }
//	AX_INLINE	void	operator=	( const axInitList<T>& rhs )		{ set(rhs); }

	AX_INLINE	bool	operator==	( Span r ) const { return span() == r; }
	AX_INLINE	bool	operator!=	( Span r ) const { return span() != r; }

					void	move		( This & rhs )						{ clear(); appendRange(ax_move(rhs)); }

	template<class V>
	AX_INLINE	axInt	find		( const V& v ) const;

	template<class V>
	AX_INLINE	bool	contains	( const V& v ) const				{ return find(v) >= 0; }

	template<class V>
	AX_INLINE	axInt	findAndRemove	( const V& v );

					bool	uniqueAppend	( const T& v ) { if (contains(v)) return false; append(v); return true; }

					template<class CompareFunc>
					axInt	findMin_	(CompareFunc func) const { return span().findMin_(func); }

					axInt	findMin		() const { return span().findMin(); }
					axInt	findMax		() const { return span().findMax(); }

					template<class CompareFunc>
					void	sort_(CompareFunc func);

					void	sort		() { sort_(ax_compare<T>); }


	void			setAllocator(axAllocator* a)	{ _allocator = a; }
	axAllocator*	allocator() const				{ return _allocator ? _allocator : ax_default_array_allocator(); }

	void onFormat(axFormat& f) const { return span().onFormat(f); }
	void onJsonWriter(axJsonWriter& wr) const { axSpan_onJsonWriter(wr, span()); }

	virtual bool isDataMovable() const = 0;

protected:
	AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

	template< class... Args >
	void	_resize		( axInt newSize, Args&&... newElementConstructArgs );

	axIArray() = default;

	virtual	MutSpan	onMalloc		( axInt req_size ) = 0;
	virtual	void	onFree			( T* p ) = 0;
			void	_reserve		( axInt newSize );

	T*		_data = nullptr;
	axInt	_size = 0;
	axInt	_capacity = 0;
	axAllocator* _allocator = nullptr;
};

namespace axTypeTraits {
	template<class T>	struct spanType<    axIArray<T> > { using Type = axSpan<T>; };
	template<class T>	struct mutSpanType< axIArray<T> > { using Type = axMutSpan<T>; };
}

template<class T> AX_INLINE 
void  ax_bzero (axIArray<T> & arr) {
	ax_bzero(arr.span());
}

template<class T> 
template<class V> AX_INLINE
axInt axIArray<T>::findAndRemove(const V& v) {
	axInt i = 0;
	for (auto& e : *this) {
		if (e == v) {
			removeAt(i);
			return i;
		}
		i++;
	}
	return -1;
}

template<class T> 
template<class V> AX_INLINE
axInt axIArray<T>::find(const V& v) const {
	auto* e = end();
	for (auto* p = _data; p < e; p++) {
		if (*p == v) return p - _data;
	}
	return -1;
}

template< class T > AX_INLINE	
void axIArray<T>::removeAt(axInt i) {
	_checkBound(i);
	auto* p = _data + i;
	auto* e = _data + _size - 1;
	for (; p < e; p++) {
		*p = ax_move(p[1]);
	}
	popBack();
}

template<class T>
template<class R> AX_INLINE
void axIArray<T>::appendRangeByCopy(const R& rhs) {
	for (auto& it : rhs) {
		append(it);
	}
}

template<class T>
template<class R> AX_INLINE
void axIArray<T>::appendRangeByMove(R& rhs) {
	for (auto& it : rhs) {
		append(ax_move(it));
	}
}

template< class T > AX_INLINE
axIArray<T>::~axIArray() {
	AX_ASSERT_MSG(_data == nullptr, "axIArray data is not freed");
}

template<class T> AX_INLINE
void	axIArray<T>::clearAndFree() {
	clear();
	if( _data ) {
		onFree( _data );
		_data = nullptr;
		_capacity = 0;
	}
}

template< class T >
template< class... Args > inline
void	axIArray<T>::resize( axInt newSize, Args&&... newElementConstructArgs ) {
	if( newSize < 0 ) throw axError_Undefined(AX_LOC);
	if( newSize == _size ) return;
	_resize(newSize, ax_forward(newElementConstructArgs)...);
}

template< class T >
template< class... Args > AX_INLINE
void	axIArray<T>::_resize( axInt newSize, Args&&... newElementConstructArgs ) {
	auto oldSize = _size;
	if( newSize <  oldSize ) {
		auto dst = _data + newSize;
		auto n   = oldSize  - newSize;
		axArrayUtil::destructor ( dst, n );
	}else{
		reserve( newSize );
		axArrayUtil::constructor( _data + oldSize, newSize - oldSize, ax_forward(newElementConstructArgs)... );
	}
	_size = newSize;
}

template< class T >
template< class... Args > AX_INLINE
void	axIArray<T>::resizeNoCopy( axInt newSize, Args&&... newElementConstructArgs ) {
	if (newSize > _capacity) {
		clear();
	}
	resize(newSize, ax_forward(newElementConstructArgs)...);
}

template< class T > AX_INLINE
void	axIArray<T>::reserve( axInt newSize ) {
	if( newSize <= _capacity ) return;
	_reserve( newSize );
}

template< class T > AX_INLINE
void	axIArray<T>::append( const T& v ) {
	reserve( _size + 1 );
	ax_call_constructor( _data + _size, v );
	_size++;
}

template< class T > AX_INLINE
void	axIArray<T>::append	( T && v ) {
	reserve( _size + 1 );
	ax_call_constructor( _data + _size, ax_move(v) );
	_size++;
}

template< class T > inline
void	axIArray<T>::appendRange(Span rhs) {
	AX_ASSERT_MSG(!span().isOverlapped(rhs), "axIArray::extend() memory overlapped");

	if(rhs.size() <= 0) return;
	reserve( _size + rhs.size() );
	axArrayUtil::copyConstructor( _data + _size, rhs.data(), rhs.size() );
	_size += rhs.size();
}

template< class T > inline
void	axIArray<T>::appendRange (This && rhs) {
	if (rhs._size <= 0) return;
	AX_ASSERT_MSG(!span().isOverlapped(rhs.span()), "axIArray::extend() memory overlapped");

	if (_size == 0 && rhs.isDataMovable()) {
		if (this == &rhs) return;
		if (_data) {
			onFree(_data);
		}
		_data		  = rhs._data;
		_size		  = rhs._size;
		_capacity	  = rhs._capacity;

		rhs._data		= nullptr;
		rhs._size		= 0;
		rhs._capacity	= 0;

	} else {
		reserve( _size + rhs.size() );
		axArrayUtil::moveConstructor( _data + _size, rhs._data, rhs._size );
		_size += rhs._size;
		rhs.clear();
	}
}

template< class T > inline
void	axIArray<T>::_reserve( axInt newSize ) {
	auto buf = onMalloc( newSize );
	if( buf.data() != _data ) {
		axArrayUtil::moveConstructorAndDestructor( buf.data(), _data, _size );
		if( _data ) {
			onFree( _data );
		}
		_data = buf.data();
	}
	_capacity = buf.size();
}


template<class T> 
template<class CompareFunc> inline
void	axIArray<T>::sort_(CompareFunc func) {
	// simple sorting, prevent move data if possible

	for (axInt i = 0; i < _size; i++) {
		axInt m = sliceFrom(i).findMin_(func);
		if (m == i) continue;
		ax_swap(at(m), at(i));
	}
}


