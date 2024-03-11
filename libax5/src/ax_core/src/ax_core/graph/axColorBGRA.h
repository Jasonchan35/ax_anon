#pragma once

#include "axColorBGRA.h"

template<class T>
class axColorBGRA_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 4;
	static constexpr axColorType kColorType = axColorType_BGRA<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::BGRA;
	static constexpr axInt kAlphaBits = ax_sizeof(T);

	T b,g,r,a;

	AX_STRUCT_BEGIN(axColorBGRA_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(b, g, r, a)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---
	static constexpr This kBlack	() { return This(kElemMin(),  kElemMin(),  kElemMin()); }
	static constexpr This kWhite	() { return This(kElemMax(),  kElemMax(),  kElemMax() ); }
	static constexpr This kGray		() { return This(kElemHalf(), kElemHalf(), kElemHalf()); }
//---
	static constexpr This kRed		() { return This(kElemMin(),  kElemMin(),  kElemMax()); }
	static constexpr This kGreen	() { return This(kElemMin(),  kElemMax(),  kElemMin()); }
	static constexpr This kBlue		() { return This(kElemMax(),  kElemMin(),  kElemMin()); }
//---
	static constexpr This kYellow	() { return This(kElemMin(),  kElemMax(),  kElemMax()); }
	static constexpr This kCyan		() { return This(kElemMax(),  kElemMax(),  kElemMin()); }
	static constexpr This kMagenta	() { return This(kElemMax(),  kElemMin(),  kElemMax()); }
//---

	AX_INLINE explicit axColorBGRA_() = default;
	AX_INLINE          axColorBGRA_(T b_, T g_, T r_, T a_ = kElemMax()) { set(b_,g_,r_,a_); }

	AX_INLINE		 T* data()			{ return &b; }
	AX_INLINE const T* data() const	{ return &b; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&b, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&b, kElementCount); }

	AX_INLINE void set(T b_, T g_, T r_, T a_ = kElemMax()) { b=b_; g=g_; r=r_; a=a_; }

	AX_INLINE This	operator+	(const This& rhs) const { return This(b+rhs.b, g+rhs.g, r+rhs.r, a+rhs.a); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(b-rhs.b, g-rhs.g, r-rhs.r, a-rhs.a); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(b,rhs.b), _mul(g,rhs.g), _mul(r,rhs.r), _mul(a,rhs.a)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(b,rhs.b), _div(g,rhs.g), _div(r,rhs.r), _div(a,rhs.a)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return b == rhs.b && g == rhs.g && r == rhs.r && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return b != rhs.b || g != rhs.g || r != rhs.r || a != rhs.a; }

	This toPremultipliedAlpha	() const {
		auto scalar = ElemUtil::toDouble(a);
		return This(ElemUtil::mulDouble(r, scalar),
					ElemUtil::mulDouble(g, scalar),
					ElemUtil::mulDouble(b, scalar),
					a);
	}
	This toNonPremultipliedAlpha() const {
		auto scalar = ax_div(1.0, ElemUtil::toDouble(a));
		return This(ElemUtil::mulDouble(r, scalar),
					ElemUtil::mulDouble(g, scalar),
					ElemUtil::mulDouble(b, scalar),
					a);
	}

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
	struct MakeHelper<axColorBGRA_<SRC_E>, SRC_E> {
		static This make(const axColorBGRA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.b),
				ElemUtil::to<T>(src.g),
				ElemUtil::to<T>(src.r),
				ElemUtil::to<T>(src.a)
			);
		}
	};

	AX_INLINE static T _mul(T a, T v) { return ElemUtil::mul(a,v); }
	AX_INLINE static T _div(T a, T v) { return ElemUtil::div(a,v); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorBGRA_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorBGRA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorBGRA_<T> & v) {
	auto span = v.span();
	se.io(span);
}
