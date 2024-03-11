#pragma once
#include "axIArray.h"

template< class T, axInt N = axArray_kDefaultSize(T) >
class axArray : public axIArray<T>, private axLocalBuffer< T, N >  {
	using LocalBuf = axLocalBuffer< T, N >;
	using Base = axIArray< T >;
	using This = axArray< T, N >;
public:
	static const axInt kLocalBufSize = LocalBuf::kLocalBufSize;

	using Base::allocator;
	using MutSpan = typename Base::MutSpan;
	using Span    = typename Base::Span;

	virtual	~axArray() { Base::clearAndFree(); }

	explicit axArray()								{ Base::_data = LocalBuf::localBufPtr(); Base::_capacity = N; }
	explicit axArray(Span rhs)						{ Base::set(rhs); }
	explicit axArray(const axArray      &  rhs  )	{ Base::set(rhs);  }
	explicit axArray(const axIArray<T>  &  rhs  )	{ Base::set(rhs);  }
	explicit axArray(      axIArray<T>  && rhs  )	{ Base::move(rhs); }

	explicit axArray(axAllocator* allocator,       axIArray<T>  && rhs  ) { Base::setAllocator(allocator); Base::move(rhs); }

	template<axInt M>
	explicit axArray(  axArray<T,M>  && rhs )	{ Base::move(rhs); }

	void operator=	( axIArray<T>    && rhs )	{ Base::move(rhs); }
	void operator=	( Span rhs )				{ Base::set(rhs); }
	void operator=	( const axArray& rhs)		{ Base::set(rhs); }

	template<axInt M>
	void operator=	( const axArray<T,M>& rhs)	{ Base::set(rhs); }

	operator MutSpan()		{ return Base::mutSpan(); }
	operator Span() const	{ return Base::span(); }

	axIArray<T> &	asIArray() & { return *this; }

	virtual bool isDataMovable() const override { return Base::_data != LocalBuf::localBufPtr(); }

protected:

	virtual	MutSpan onMalloc	( axInt reqSize ) override {
		//std::cout << this << " onMalloc " << reqSize <<"\n";

		if( reqSize <= N ) {
			return MutSpan(LocalBuf::localBufPtr(), N);
		}else{
			auto oldCap = Base::capacity();
			auto newCapacity = ax_max(reqSize, ax_next_pow2_half(oldCap + 1)); // reqSize or align to next pow of 2.5
			auto alignedSize = ax_alignof(T);
			auto* p = ax_malloc(T, newCapacity, alignedSize);
			return MutSpan(p, newCapacity);
		}
	}

	virtual	void	onFree		( T* p ) override {
		//std::cout << this << " onFree   " << "\n";
		if( p != LocalBuf::localBufPtr() ) {
			ax_free(p);
		}
	}
};

namespace axTypeTraits {
	template<class T, axInt N>	struct spanType<      axArray<T, N> > { using Type = axSpan<T>; };
	template<class T, axInt N>	struct mutSpanType<   axArray<T, N> > { using Type = axMutSpan<T>; };
	template<class T, axInt N>	struct interfaceType< axArray<T, N> > { using Type = axIArray<T>; };
}

template<class T, axInt N> AX_INLINE 
void  ax_bzero (axArray<T, N> & arr) {
	ax_bzero(arr.span());
}

//template< class T, axInt N > inline
//std::ostream& operator << ( std::ostream & s, const axArray<T, N> & v ) { return s << v.view(); }

