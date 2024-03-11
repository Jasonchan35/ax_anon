#pragma once

//  ax_core
//
//  Created by Jason Chan on Mar 16, 2015.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#include "axArrayEnumerator.h"
#include "../math/axRange.h"

template<class T> class axMutSpan;
template<class T> using axSpan = axMutSpan<const T>;

template<class T> void axSpan_onJsonWriter(axJsonWriter& wr, const axSpan<T>& value);
template<class T> void axSpan_onJsonReader(axJsonReader& rd,       axSpan<T>& value);

template<class T> class axInitList;

using axByteSpan    = axSpan<    axByte >;
using axByteMutSpan = axMutSpan< axByte >;

using axIntSpan		= axSpan<    axInt >;
using axIntMutSpan	= axMutSpan< axInt >;

class axFormat;

template<class T>
class axMutSpan { //Copyable
	typedef axMutSpan		This;
public:
	using MutSpan = axMutSpan<T>;
	using    Span = axMutSpan<const T>;

	using MutByte = typename axTypeTraits::selectType< const axByte, axByte, axTypeTraits::isConst<T>::value >::Type;
	using ByteMutSpan = axMutSpan<MutByte>;

	axMutSpan() = default;
	axMutSpan(const axTag::EmptyT&) {}

	template<class R>
	axMutSpan(R* data, axInt size) : _data(data), _size(size) {}

	template<class R, axInt N>
	axMutSpan(R (&data)[N]) : _data(data), _size(N) {}

	template<class R, std::enable_if_t<axTypeTraits::isConvertible<R*, T*>::value, int> = 0>
	axMutSpan(R& r) : _data(&r), _size(1) {}

	//		axMutSpan	(axMutSpan<  axTypeTraits::removeConst<T> > r) { operator=(r); }
	//void	operator=	(axMutSpan<  axTypeTraits::removeConst<T> > r) { *this = r.constSpan(); }

	MutSpan	 span		()			{ return MutSpan(_data, _size); }
	Span	 span		() const	{ return    Span(_data, _size); }
	Span	 constSpan	() const	{ return    Span(_data, _size); }

	operator Span		() const	{ return span(); }

	AX_INLINE	 	  T &	operator[]	( axInt i )			{ return at(i); }
	AX_INLINE	const T &	operator[]	( axInt i ) const	{ return at(i); }

	AX_INLINE	 	  T &	at			( axInt i )			{ _checkBound(i); return _data[i]; }
	AX_INLINE	const T &	at			( axInt i ) const	{ _checkBound(i); return _data[i]; }

	AX_INLINE 		  T &	back		()					{ return at( _size-1 ); }
	AX_INLINE 	const T &	back		() const			{ return at( _size-1 ); }

	AX_INLINE 		  T &	back		( axInt i )			{ return at( _size-i-1 ); }
	AX_INLINE 	const T &	back		( axInt i ) const	{ return at( _size-i-1 ); }

	AX_INLINE 		  T &	unsafe_at	( axInt i ) 		{ _debug_checkBound(i); return _data[i]; }
	AX_INLINE 	const T &	unsafe_at	( axInt i ) const	{ _debug_checkBound(i); return _data[i]; }

	AX_INLINE 		  T &	unsafe_back	( axInt i )			{ return unsafe_at( _size - i - 1 ); }
	AX_INLINE 	const T &	unsafe_back	( axInt i )  const	{ return unsafe_at( _size - i - 1 ); }

	AX_INLINE	MutSpan	slice		(axInt offset, axInt size);
	AX_INLINE	MutSpan	slice		(axIntRange range)					{ return slice(range.start, range.size); }
	AX_INLINE	MutSpan	sliceFrom	(axInt offset)						{ return slice(offset, _size - offset); }
	AX_INLINE	MutSpan	sliceBack	(axInt n)							{ return slice(_size - n, n); }
	AX_INLINE	MutSpan	sliceLess	(axInt n)							{ return slice(0, _size - n); }

	AX_INLINE	bool	inBound		(axInt  i) const					{ return i >= 0 && i < _size; }
	AX_INLINE	bool	isOverlapped(Span rhs) const					{ return axArrayUtil::isOverlapped(_data, _size, rhs.data(), rhs.size()); }

	AX_INLINE	axInt	size		() const		{ return _size; }
	AX_INLINE	axInt	sizeInBytes	() const		{ return _size * ax_sizeof(T); }

	AX_INLINE	static MutSpan	s_fromByteSpan		(axByteMutSpan from)	{ return MutSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof(T)); }
	AX_INLINE	static Span		s_fromByteSpan		(axByteSpan    from)	{ return    Span(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof(T)); }

	AX_INLINE	void			fromByteSpan		(axByteMutSpan from)	{ *this = s_fromByteSpan(from); }
	AX_INLINE	void			fromByteSpan		(axByteSpan    from)	{ *this = s_fromByteSpan(from); }

	AX_INLINE	ByteMutSpan		toByteMutSpan		() const				{ return ByteMutSpan(reinterpret_cast<     MutByte*>(_data), sizeInBytes()); }
	AX_INLINE	axByteSpan		toByteSpan			() const				{ return  axByteSpan(reinterpret_cast<const axByte*>(_data), sizeInBytes()); }

	AX_INLINE 			T*	data		()			{ return ax_const_cast(_data); }
	AX_INLINE	const 	T*	data		() const	{ return _data; }

	AX_INLINE		void	setPtr		(T* data, axInt size)	{ _data = data; _size = size; }

	AX_INLINE		void	fillValues	(const T& v);

	AX_INLINE		void	copyValues	(axSpan<T> v, axInt offset = 0) {
		if (offset < 0 || offset + v.size() > _size) throw axArrayError_OutOfRange();
		axArrayUtil::copy(_data + offset, v.data(), v.size());
	};

	AX_INLINE		void	setValues	(const std::initializer_list<T>& list);

	AX_INLINE		bool	operator==	(MutSpan r) const	{
		return _size == r.size() && axArrayUtil::equals(_data, r.data(), size());
	};
	AX_INLINE		bool	operator!=	(MutSpan r) const	{ 
		return _size != r.size() || !axArrayUtil::equals(_data, r.data(), size());
	}

	bool operator< (Span r) const {
		for (axInt i = 0; i < _size; i++) {
			if (i >= r.size()) return false;
			auto& a = at(i);
			auto& b = r[i];
			if (a < b) return true;
			if (a > b) return false;
		}
		return false;
	}

	explicit operator bool() const { return _size > 0; }

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

	template<class CompareFunc>
	axInt findMin_(CompareFunc func) const { return _findMin(func); }

	axInt findMin() const { return _findMin([](auto& a, auto& b) -> bool { return a < b; });}
	axInt findMax() const { return _findMin([](auto& a, auto& b) -> bool { return a > b; });}

    //---------------
    
	// implement in axFormatParam.h
	void onFormat(axFormat& f) const;
	void onJsonWriter(axJsonWriter& wr) const { axSpan_onJsonWriter(wr, constSpan()); }

    AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

private:
	T*		_data = nullptr;
	axInt	_size = 0;

	template<class CompareFunc>
	axInt _findMin(CompareFunc func) const {
		if (_size == 0) return -1;

		axInt minIndex = 0;
		for (axInt i = 1; i <_size; i++) {
			if (func(_data[i], _data[minIndex]))
				minIndex = i;
		}
		return minIndex;
	}
};

template<class T> AX_INLINE 
void  ax_bzero (axMutSpan<T> span) {
	AX_STATIC_ASSERT(axTypeTraits::isPod<T>::value);
	ax_bzero(span.data(), span.sizeInBytes());
}

template<class T> AX_INLINE		
void axMutSpan<T>::fillValues(const T& v) {
	T* p = _data;
	T* end = _data + _size;
	for (; p < end; p++) {
		*p = v;
	}
}

template<class T> AX_INLINE
void axMutSpan<T>::setValues(const std::initializer_list<T>& list) {
	axSpan<T> span(list.begin(), static_cast<axInt>(list.size()));
	copyValues(span);
}

template<class T> AX_INLINE 
axMutSpan<T> axMutSpan<T>::slice(axInt offset, axInt size) {
	if (offset < 0 || size < 0 || offset + size > _size) {
		throw axError_SliceOutOfRange();
	}
	return MutSpan(_data + offset, size);
}

//--------------------------

template<class T, axInt N> AX_INLINE
axMutSpan<T>  axMutSpan_make(T (&p)[N]) { return axMutSpan<T>(p, N); }

template<class T> AX_INLINE
axMutSpan<T>  axMutSpan_make(T* p, axInt n) { return axMutSpan<T>(p, n); }

template<class T> AX_INLINE
axMutSpan<T>  axMutSpan_make(T& p) { return axMutSpan<T>(&p, 1); }

//--------------------------

template<class T, axInt N> AX_INLINE
axSpan<T>  axSpan_make(T (&p)[N]) { return axSpan<T>(p, N); }

template<class T> AX_INLINE
axSpan<T>  axSpan_make(T* p, axInt n) { return axSpan<T>(p, n); }

template<class T> AX_INLINE
axSpan<T>  axSpan_make(T& p) { return axSpan<T>(&p, 1); }
