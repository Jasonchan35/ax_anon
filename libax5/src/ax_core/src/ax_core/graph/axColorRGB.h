#pragma once

#include "axColorRG.h"

template<class T>
class axColorRGB_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 3;
	static constexpr axColorType kColorType = axColorType_RGB<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::RGB;
	static constexpr axInt kAlphaBits = 0;

	T r,g,b;

	AX_STRUCT_BEGIN(axColorRGB_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(r, g, b)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_IO(se, r,g,b);
	}

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
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


	AX_INLINE explicit axColorRGB_() = default;
	AX_INLINE          axColorRGB_(T v) { set(v,v,v); }
	AX_INLINE          axColorRGB_(T r_, T g_, T b_) { set(r_,g_,b_); }

	AX_INLINE		 T* data()			{ return &r; }
	AX_INLINE const T* data() const	{ return &r; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&r, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&r, kElementCount); }

	AX_INLINE void set(T r_, T g_, T b_) { r=r_; g=g_; b=b_; }

	AX_INLINE axColorRGBA_<T>	rgba() const;
	AX_INLINE axColorL_<T>		toColorL() const;
	AX_INLINE axColorLA_<T>	toColorLA() const;

	AX_INLINE T luma() const { return ElemUtil::grayscale(r,g,b); }
	This grayscale() const { auto c = luma(); return This(c, c, c); }

	AX_INLINE This	operator+	(const This& rhs) const { return This(r+rhs.r, g+rhs.g, b+rhs.b); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(r-rhs.r, g-rhs.g, b-rhs.b); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(r,rhs.r), _mul(g,rhs.g), _mul(b,rhs.b)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(r,rhs.r), _div(g,rhs.g), _div(b,rhs.b)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return r != rhs.r || g != rhs.g || b != rhs.b; }

	void toHexString(axIString& s) const;

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.rgb());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorRGB_<SRC_E>, SRC_E> {
		static This make(const axColorRGB_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.r),
				ElemUtil::to<T>(src.g),
				ElemUtil::to<T>(src.b)
			);
		}
	};

	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

template<class DST, class SRC> AX_INLINE 
axColorRGB_<DST> axColorRGB_make(const axColorRGB_<SRC>& rhs) {
	using ElemUtil = axColorElementUtil;
	return axColorRGB_<DST>(	ElemUtil::to<DST>(rhs.r),
								ElemUtil::to<DST>(rhs.g),
								ElemUtil::to<DST>(rhs.b));
}

#define	AX_MACRO_OP(T, VALUE, SUFFIX)	\
	template<class R> AX_INLINE \
	axColorRGB_<T> axColorRGB ## SUFFIX ## _make(const axColorRGB_<R>& c) { \
		return axColorRGB_make<T, R>(c); \
	} \
//-------------
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorRGB_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorRGB_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorRGB_<T> & v) {
	auto span = v.span();
	se.io(span);
}
