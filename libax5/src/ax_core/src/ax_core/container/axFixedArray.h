#pragma once

#include "axLocalBuffer.h"
#include "axSpan.h"

template<class T, axInt N>
class axFixedArrayLocalBuffer {
public:
			T* data()		{ return _data; }
	const	T* data() const { return _data; }

	AX_INLINE bool operator==(const axFixedArrayLocalBuffer& r) const { 
		return _data == r._data; 
	}

	AX_INLINE bool operator!=(const axFixedArrayLocalBuffer& r) const { 
		return _data != r._data; 
	}

protected:
	T _data[N];
};

template<class T>
class axFixedArrayLocalBuffer<T, 0> {
public:
			T* data()		{ return nullptr; }
	const	T* data() const { return nullptr; }

	AX_INLINE bool operator==(const axFixedArrayLocalBuffer& r) const { 
		return true; 
	}
	AX_INLINE bool operator!=(const axFixedArrayLocalBuffer& r) const { 
		return false; 
	}
};

//---------------------
template<class T, axInt N>
class axFixedArray : public axFixedArrayLocalBuffer<T, N> {
	using This = axFixedArray;
	using Base = axFixedArrayLocalBuffer<T, N>;
public:
	using Base::data;

	static const axInt kSize = N;

	typedef axMutSpan<T>		MutSpan;
	typedef axSpan<T>			Span;

	MutSpan	  span		()			{ return MutSpan(data(), kSize); }
	   Span	  span		() const	{ return    Span(data(), kSize); }
	   Span	  constSpan	() const	{ return    Span(data(), kSize); }

	operator  MutSpan	()			{ return span(); }
	operator  Span		() const	{ return span(); }

	AX_INLINE	 	  T &	operator[]	( axInt i )			{ return at(i); }
	AX_INLINE	const T &	operator[]	( axInt i ) const	{ return at(i); }

	AX_INLINE	 	  T &	at			( axInt i )			{ _checkBound(i); return data()[i]; }
	AX_INLINE	const T &	at			( axInt i ) const	{ _checkBound(i); return data()[i]; }

	//-----------------------
	AX_INLINE		axInt	size		() const			{ return N; }
	AX_INLINE		bool	inBound		( axInt  i ) const	{ return i >= 0 && i < N; }

	AX_INLINE 		  T*	data		()					{ return Base::data(); }
	AX_INLINE	const T*	data		() const			{ return Base::data(); }

	//------- forward View functions ----------------
	AX_INLINE		MutSpan	slice		( axInt offset, axInt size )		{ return span().slice(offset, size); }
	AX_INLINE		Span	slice		( axInt offset, axInt size ) const	{ return span().slice(offset, size); }

	AX_INLINE		MutSpan	sliceFrom	( axInt offset )					{ return span().sliceFrom(offset); }
	AX_INLINE		Span	sliceFrom	( axInt offset ) const				{ return span().sliceFrom(offset); }

	AX_INLINE		MutSpan	sliceBack	( axInt offset )					{ return span().sliceBack(offset); }
	AX_INLINE		Span	sliceBack	( axInt offset ) const				{ return span().sliceBack(offset); }

	AX_INLINE		void	copyValues	(axSpan<T> v, axInt offset = 0)		{ span().copyValues(); }
	AX_INLINE		void	fillValues	( const T& v )						{ span().fillValues(v); }

	//---------------
    template<class R> using Enumerator_    = axArrayEnumerator<R>;
    template<class R> using RevEnumerator_ = axArrayRevEnumerator<R>;

	using Enumerator			= Enumerator_<T>;
	using ConstEnumerator		= Enumerator_<const T>;
	using RevEnumerator			= RevEnumerator_<T>;
	using ConstRevEnumerator	= RevEnumerator_<const T>;
	using Iterator				= T*;
	using ConstIterator			= const T*;
    
	Enumerator			each	()			{ return Enumerator(		data(), data() + N); }
	ConstEnumerator		each	() const	{ return ConstEnumerator(	data(), data() + N); }

	RevEnumerator		revEach	()			{ return RevEnumerator(		data(), data() + N); }
	ConstRevEnumerator	revEach	() const	{ return ConstRevEnumerator(data(), data() + N); }

	Iterator		begin	()			{ return data(); }
    ConstIterator	begin	() const	{ return data(); }
    Iterator		end		()			{ return data() + N; }
    ConstIterator	end		() const	{ return data() + N; }

	void onFormat(axFormat& f) const { return constSpan().onFormat(f); }

private:
	AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}
};

namespace axPtrUtil {
	template< class Obj, class T, axInt N > constexpr
	intptr_t memberOffset(axFixedArray<T,N> Obj::*ptrToMember, axInt elementIndex) {
		auto* c = reinterpret_cast<Obj*>(0);
		auto* m = &(c->*ptrToMember)[elementIndex];
		return reinterpret_cast<intptr_t>(m);
	}
} //namespace axPtrUtil

