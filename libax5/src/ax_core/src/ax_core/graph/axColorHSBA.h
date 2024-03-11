#pragma once

#include "axColorHSBA.h"

template<class T>
class axColorHSBA_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 4;
	static constexpr axColorType kColorType = axColorType_HSBA<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::HSBA;
	static constexpr axInt kAlphaBits = ax_sizeof(T);

	T h,s,b,a;

	AX_STRUCT_BEGIN(axColorHSBA_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(h, s, b, a)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---

	AX_INLINE explicit axColorHSBA_() = default;
	AX_INLINE          axColorHSBA_(T h_, T s_, T v_, T a_ = kElemMax()) { set(h_,s_,v_,a_); }

	AX_INLINE		 T* data()			{ return &h; }
	AX_INLINE const T* data() const	{ return &h; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&h, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&h, kElementCount); }

	AX_INLINE void set(T h_, T s_, T b_, T a_ = kElemMax()) { h=h_; s=s_; b=b_; a=a_; }

	AX_INLINE This	operator+	(const This& rhs) const { return This(h+rhs.h, s+rhs.s, b+rhs.b, a+rhs.a); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(h-rhs.h, s-rhs.s, b-rhs.b, a-rhs.a); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(h,rhs.h), _mul(s,rhs.s), _mul(b,rhs.b), _mul(a,rhs.a)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(h,rhs.h), _div(s,rhs.s), _div(b,rhs.b), _div(a,rhs.a)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return h == rhs.h && s == rhs.s && b == rhs.b && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return h != rhs.h || s != rhs.s || b != rhs.b || a != rhs.a; }

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.rgba());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorHSBA_<SRC_E>, SRC_E> {
		static This make(const axColorHSBA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.h),
				ElemUtil::to<T>(src.s),
				ElemUtil::to<T>(src.b),
				ElemUtil::to<T>(src.a)
			);
		}
	};

	AX_INLINE static T _mul(T a, T v) { return ElemUtil::mul(a,v); }
	AX_INLINE static T _div(T a, T v) { return ElemUtil::div(a,v); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorHSBA_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorHSBA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorHSBA_<T> & v) {
	auto span = v.span();
	se.io(span);
}
