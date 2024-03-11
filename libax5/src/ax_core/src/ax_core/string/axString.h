#pragma once
#include "axIString.h"

template< class T, axInt N = axString_kDefaultSize >
class axStringT : public axIStringT<T>, axLocalBuffer<T, N+1> {
	using Base = axIStringT<T>;
	using This = axStringT<T, N>;
	using LocalBuf = axLocalBuffer<T, N+1>;
public:
	static const axInt kLocalBufSize = LocalBuf::kLocalBufSize;

    using View    = typename Base::View;
    using MutView = typename Base::MutView;

	AX_INLINE explicit axStringT	() { Base::reserve(N); }
	AX_INLINE explicit axStringT	(axAllocator* allocator)		{ Base::setAllocator(allocator); }
	//AX_INLINE explicit axStringT	( const View &  rhs ) { Base::set(rhs); } //avoid copy happened in axString as function parameter
	AX_INLINE          axStringT	( const axStringT     &  rhs )			{ Base::set(rhs); }
	AX_INLINE          axStringT	(       axStringT     && rhs ) noexcept	{ Base::move(rhs.asIString()); }
	AX_INLINE explicit axStringT	(       axIStringT<T> && rhs ) noexcept	{ Base::move(rhs); }

	template<axInt M>
	AX_INLINE explicit axStringT	(       axStringT<T, M> && rhs ) { Base::move(rhs.asIString()); }

	template<class... ARGS>
	AX_INLINE explicit axStringT	(ARGS&&... args) { Base::append(ax_forward(args)...); }

	virtual	~axStringT() { Base::clearAndFree(); }

	AX_INLINE	void resizeToLocalBufSize() { Base::resize(N); }

	AX_INLINE void	operator=	( View     rhs ) { Base::set(rhs); }
	AX_INLINE void	operator=	( axIStringT<T> && rhs ) { Base::move(rhs); }

	template<axInt M>
	AX_INLINE void	operator=	( const axStringT<T, M> & rhs ) { Base::set(rhs); }
	AX_INLINE void	operator=	( const axStringT<T, N> & rhs ) { Base::set(rhs); }

	template<axInt M>
	AX_INLINE void	operator=	( axStringT<T, M> && rhs ) { Base::move(rhs.asIString()); }

	AX_INLINE 			axIStringT<T> &		asIString() &		{ return *this; }
	AX_INLINE const	axIStringT<T> &		asIString() const &	{ return *this; }

	virtual bool isDataMovable() const override { return Base::data() != LocalBuf::localBufPtr(); }

	template<class FMT, class... ARGS>
	static This s_format(const FMT& fmt, ARGS&&... args) {
		This str;
		str.format(fmt, ax_forward(args)...);
		return str;
	}

	template<class R>
	static This s_convert(const R& r) {
		This str;
		str.convert(r);
		return str;
	}	

	static This s_hex(axByteSpan src, bool upperCase = true, T separator = 0) {
		This str;
		str.appendHex(src, upperCase, separator);
		return str;
	}

protected:
	const	T*	getLocalBufferPtr() const { return LocalBuf::localBufPtr(); }
	virtual	axMutSpan<T>	onMalloc(axInt reqSize) override;
	virtual	void onFree(T* p) override;
};

namespace axTypeTraits {
	template<class T, axInt N>	struct spanType<      axStringT<T,N> > { using Type = axStrViewT<T>; };
	template<class T, axInt N>	struct mutSpanType<   axStringT<T,N> > { using Type = axStrMutViewT<T>; };
	template<class T, axInt N>	struct interfaceType< axStringT<T,N> > { using Type = axIStringT<T>; };
}

#if AX_OS_MACOSX || AX_OS_IOS
	inline NSString* ax_toNSString(axStrView16 s) {
		if (!s) return [NSString string];
		return [NSString stringWithCharacters: axCharPtrToUIntPtr(s.data())
						 length: ax_safe_static_cast<NSUInteger>(s.size())];
	}
	inline NSString* ax_toNSString(axStrView8 s) {
		axTempString16 tmp;
		tmp.setUtf(s);
		return ax_toNSString(tmp);
	}
#endif //#if AX_OS_MACOSX || AX_OS_IOS

template< class T, axInt N > inline
std::ostream& operator << ( std::ostream & s, const axStringT<T, N> & v ) {
	return s << v.c_str();
}

template<class T, axInt N>
inline axMutSpan<T> axStringT<T, N>::onMalloc(axInt reqSize) {
	if (reqSize <= N + 1) {  // +1 for the null terminator
//		printf("axString::onMalloc reqSize=%lld - localBuf(%lld)\n", reqSize, N);
		return axMutSpan<T>(LocalBuf::localBufPtr(), N);
	} else {
		auto oldCap = Base::capacity();
		auto newCapacity = ax_max(reqSize, ax_next_pow2_half(oldCap + 2)); // reqSize or align to next pow of 2.5
//		printf("axString::onMalloc reqSize=%lld - heap (%lld => %lld)\n", reqSize, oldCap, newCapacity);
		auto* p = ax_malloc(T, newCapacity, 0);
		return axMutSpan<T>(p, newCapacity);
	}
}

template<class T, axInt N>
inline void axStringT<T, N>::onFree(T* p) {
	//std::cout << this << " onFree   " << "\n";
	if (p != LocalBuf::localBufPtr()) {
		ax_free(p);
	}
}
