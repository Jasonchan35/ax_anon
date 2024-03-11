#pragma once

#include "axColorRGB.h"

template<class T>
class axColorRGBA_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 4;
	static constexpr axColorType kColorType = axColorType_RGBA<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::RGBA;
	static constexpr axInt kAlphaBits = ax_sizeof(T);

	T r,g,b,a;

	AX_STRUCT_BEGIN(axColorRGBA_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(r, g, b, a)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_IO(se, r,g,b,a);
	}

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---
	static constexpr This kZero		() { return This(T(0),T(0),T(0),T(0)); }
//---
	static constexpr This kBlack	() { return This(kElemMin(),  kElemMin(),  kElemMin()); }
	static constexpr This kWhite	() { return This(kElemMax(),  kElemMax(),  kElemMax()); }
	static constexpr This kGray		() { return This(kElemHalf(), kElemHalf(), kElemHalf()); }
//---
	static constexpr This kRed		() { return This(kElemMax(),  kElemMin(),  kElemMin()); }
	static constexpr This kGreen	() { return This(kElemMin(),  kElemMax(),  kElemMin()); }
	static constexpr This kBlue		() { return This(kElemMin(),  kElemMin(),  kElemMax()); }
//---
	static constexpr This kYellow	() { return This(kElemMax(),  kElemMax(),  kElemMin()); }
	static constexpr This kCyan		() { return This(kElemMin(),  kElemMax(),  kElemMax()); }
	static constexpr This kMagenta	() { return This(kElemMax(),  kElemMin(),  kElemMax()); }
//---

	AX_INLINE explicit axColorRGBA_() = default;
	AX_INLINE          axColorRGBA_(T r_, T g_, T b_, T a_ = kElemMax()) { set(r_,g_,b_,a_); }

	AX_INLINE		 T* data()			{ return &r; }
	AX_INLINE const T* data() const	{ return &r; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&r, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&r, kElementCount); }

	AX_INLINE void set(T r_, T g_, T b_, T a_ = kElemMax()) { r=r_; g=g_; b=b_; a=a_; }
	AX_INLINE void setToDefaultValue()	{ set(kElemMax(), kElemMax(), kElemMax(), kElemMax()); }

	AX_INLINE axColorRGB_<T>	rgb() const		{ return axColorRGB_<T>(r, g, b); }
	AX_INLINE axColorL_<T>		toColorL() const;
	AX_INLINE axColorLA_<T>	toColorLA() const;

	AX_INLINE T luma() const { return ElemUtil::grayscale(r,g,b); }
	This grayscale() const { auto c = luma(); return This(c, c, c, a); }

	AX_INLINE This	operator+	(const This& rhs) const { return This(r+rhs.r, g+rhs.g, b+rhs.b, a+rhs.a); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(r-rhs.r, g-rhs.g, b-rhs.b, a-rhs.a); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(r,rhs.r), _mul(g,rhs.g), _mul(b,rhs.b), _mul(a,rhs.a)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(r,rhs.r), _div(g,rhs.g), _div(b,rhs.b), _div(a,rhs.a)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a; }

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

	void toHexString(axIString& s) const;

	template<class WEIGHT>
	void blend(const This& c, WEIGHT weight) {
		AX_STATIC_ASSERT(axTypeTraits::isFloatingPoint<WEIGHT>::value);
		WEIGHT w = weight * ElemUtil::to<WEIGHT>(c.a);
		r = ax_lerp(r, c.r, w);
		g = ax_lerp(g, c.g, w);
		b = ax_lerp(b, c.b, w);
		a = ElemUtil::add(a, c.a);
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
	struct MakeHelper<axColorRGBA_<SRC_E>, SRC_E> {
		static This make(const axColorRGBA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.r),
				ElemUtil::to<T>(src.g),
				ElemUtil::to<T>(src.b),
				ElemUtil::to<T>(src.a)
			);
		}
	};

	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorRGBA_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorRGBA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorRGBA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> AX_INLINE
axColorRGBA_<T> axColorRGB_<T>::rgba() const { return axColorRGBA_<T>(r, g, b); }


