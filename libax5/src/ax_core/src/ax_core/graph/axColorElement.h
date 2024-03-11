#pragma once

#include <ax_core/base/ax_base.h>
#include <ax_core/other/axEnum.h>
#include <ax_core/math/axMat4.h>
#include <ax_core/math/axFloat16.h>
#include <ax_core/math/axUNorm.h>
#include <ax_core/data_format/axBinDeserializer.h>
#include <ax_core/data_format/json/axJsonSerializer.h>

#include <ax_core/base/ax_base.h>
#include <ax_core/other/axEnum.h>

/*
R = Red
G = Green
B = Blue
A = Alpha

H = Hue
S = Saturation
B = Brightness

L = Luminance / Gray

f = float
b = Byte
*/

#define axColorType_Basic_EnumList(E) \
	E(RGBAd,)	E(RGBAf,)	E(RGBAh,)	E(RGBAb,)	E(RGBAs,)	\
	E(RGBd,)	E(RGBf,)	E(RGBh,)	E(RGBb,)	E(RGBs,)	\
	E(RGd,)		E(RGf,)		E(RGh,)		E(RGb,)		E(RGs,)		\
	E(Rd,)		E(Rf,)		E(Rh,)		E(Rb,)		E(Rs,)		\
	E(Ld,)		E(Lf,)		E(Lh,)		E(Lb,)		E(Ls,)		\
	E(Ad,)		E(Af,)		E(Ah,)		E(Ab,)		E(As,)		\
	E(LAd,)		E(LAf,)		E(LAh,)		E(LAb,)		E(LAs,)		\
	E(HSBAd,)	E(HSBAf,)	E(HSBAh,)	E(HSBAb,)	E(HSBAs,)	\
	E(BGRAd,)	E(BGRAf,)	E(BGRAh,)	E(BGRAb,)	E(BGRAs,)	\
//----

#define axColorType_Extra_EnumList(E) \
	E(R5G6B5,) \
	E(R5G5B5A1,) \
//-----

#define axColorType_Compressed_EnumList(E) \
	E(BC1,) \
	E(BC2,) \
	E(BC3,) \
	E(BC4,) \
	E(BC5,) \
	E(BC6h,) \
	E(BC7,) \
	\
	E(ETC,) \
	E(PVRTC,) \
	E(ASTC,) \
//-----

#define axColorType_EnumList(E) \
	E(Unknown,)	\
	axColorType_Basic_EnumList(E) \
	axColorType_Extra_EnumList(E) \
	axColorType_Compressed_EnumList(E) \
//-------
ax_ENUM_CLASS(axColorType, axUInt8);

//------------------
#define axColorModel_EnumList(E) \
	E(Unknown,) \
	E(Compressed,) \
	E(RGBA,) \
	E(RGB,) \
	E(RG,) \
	E(R,) \
	E(LA,) \
	E(L,) \
	E(A,) \
	E(HSBA,) \
	E(BGRA,) \
//-------
ax_ENUM_CLASS(axColorModel, axUInt8);

//--------------------------
template<axColorType T> struct axColorByType_ {};
template<axColorType T> using axColorByType = typename axColorByType_<T>::Type;

#define AX_MACRO_OP(E, ...) \
	template<class T> class axColor##E##_; \
	template<class T> struct axColorType_##E; \
//----
	axColorModel_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP

//-------
#define axColorElementType_EnumList(E) \
	E(axUNorm8,	 , b) \
	E(axUNorm16, , s) \
	E(axFloat16, , h) \
	E(axFloat32, , f) \
	E(axFloat64, , d) \
//-------
ax_ENUM_CLASS(axColorElementType, axUInt8);

#define AX_MACRO_OP(E, VALE, SUFFIX) \
	\
	using axColorRGBA ##SUFFIX = axColorRGBA ##_<E>; \
	using axColorRGB  ##SUFFIX = axColorRGB  ##_<E>; \
	using axColorRG   ##SUFFIX = axColorRG   ##_<E>; \
	using axColorR    ##SUFFIX = axColorR    ##_<E>; \
	using axColorLA   ##SUFFIX = axColorLA   ##_<E>; \
	using axColorL    ##SUFFIX = axColorL    ##_<E>; \
	using axColorA    ##SUFFIX = axColorA    ##_<E>; \
	using axColorHSBA ##SUFFIX = axColorHSBA ##_<E>; \
	using axColorBGRA ##SUFFIX = axColorBGRA ##_<E>; \
	\
	template<> struct axColorType_RGBA <E> { constexpr static axColorType value = axColorType::RGBA  ##SUFFIX; }; \
	template<> struct axColorType_RGB  <E> { constexpr static axColorType value = axColorType::RGB   ##SUFFIX; }; \
	template<> struct axColorType_RG   <E> { constexpr static axColorType value = axColorType::RG    ##SUFFIX; }; \
	template<> struct axColorType_R    <E> { constexpr static axColorType value = axColorType::R     ##SUFFIX; }; \
	template<> struct axColorType_LA   <E> { constexpr static axColorType value = axColorType::LA    ##SUFFIX; }; \
	template<> struct axColorType_L    <E> { constexpr static axColorType value = axColorType::L     ##SUFFIX; }; \
	template<> struct axColorType_A    <E> { constexpr static axColorType value = axColorType::A     ##SUFFIX; }; \
	template<> struct axColorType_HSBA <E> { constexpr static axColorType value = axColorType::HSBA  ##SUFFIX; }; \
	template<> struct axColorType_BGRA <E> { constexpr static axColorType value = axColorType::BGRA  ##SUFFIX; }; \
//---
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP

#if 0
#define ax_macro_op2(ELEMENT, ELEMENT_VALUE, SUFFIX, CLASS) \
	using axColor##CLASS##SUFFIX = axColor##CLASS## _<ELEMENT>; \
	template<> struct axColorType_##CLASS<ELEMENT> { const static axColorType value = axColorType::CLASS##SUFFIX; }; \
//------------
	#define AX_MACRO_OP(CLASS, CLASS_VALUE) \
		axColorElementType_EnumList(ax_macro_op2, CLASS)
	//----
		axColorModel_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP
#undef ax_macro_op2
#endif

class axColorUnknown {
	AX_STRUCT_BEGIN(axColorUnknown, axNoBase)
	AX_STRUCT_END()
public:

	void onJsonReader(axJsonReader& rd);
	void onJsonWriter(axJsonWriter& wr) const;

	using Element = void;
	static constexpr axInt kElementCount = 0;
	static constexpr axColorType kColorType = axColorType::Unknown;
	static constexpr axColorModel kColorModel = axColorModel::Unknown;
	static constexpr axInt kAlphaBits = 0;
};

struct axColorElementUtil {
	template<class T> AX_INLINE static T kMin();
	template<class T> AX_INLINE static T kMax();
	template<class T> AX_INLINE static T kHalf();

	template<class T, class V> AX_INLINE static T to(V v);
	template<class V> AX_INLINE static double toDouble(V v) { return to<double>(v); }

	template<class T> AX_INLINE static T mulDouble(T a, double b) { return to<T>(toDouble(a) * b ); }
	template<class T> AX_INLINE static T mul(T a, T b) { return to<T>(toDouble(a) * toDouble(b)); }
	template<class T> AX_INLINE static T div(T a, T b) { return to<T>(toDouble(a) / toDouble(b)); }
	template<class T> AX_INLINE static T add(T a, T b) { return to<T>(ax_clamp01(toDouble(a) + toDouble(b))); }
	template<class T> AX_INLINE static T grayscale(T r, T g, T b);
};


template<> AX_INLINE axUNorm8  axColorElementUtil::kMin<axUNorm8 >() { return 0; }
template<> AX_INLINE axUNorm16 axColorElementUtil::kMin<axUNorm16 >() { return 0; }
template<> AX_INLINE axFloat16 axColorElementUtil::kMin<axFloat16>() { return axFloat16_make(0.0f); }
template<> AX_INLINE axFloat32 axColorElementUtil::kMin<axFloat32>() { return 0.0f; }
template<> AX_INLINE axFloat64 axColorElementUtil::kMin<axFloat64>() { return 0.0; }

template<> AX_INLINE axUNorm8  axColorElementUtil::kMax<axUNorm8 >() { return 0xFF; }
template<> AX_INLINE axUNorm16 axColorElementUtil::kMax<axUNorm16 >() { return 65535; }
template<> AX_INLINE axFloat16 axColorElementUtil::kMax<axFloat16>() { return axFloat16_make(1.0f); }
template<> AX_INLINE axFloat32 axColorElementUtil::kMax<axFloat32>() { return 1.0f; }
template<> AX_INLINE axFloat64 axColorElementUtil::kMax<axFloat64>() { return 1.0; }

template<> AX_INLINE axUNorm8  axColorElementUtil::kHalf<axUNorm8 >() { return 0x7F; }
template<> AX_INLINE axUNorm16 axColorElementUtil::kHalf<axUNorm16 >() { return 32767; }
template<> AX_INLINE axFloat16 axColorElementUtil::kHalf<axFloat16>() { return axFloat16_make(0.5f); }
template<> AX_INLINE axFloat32 axColorElementUtil::kHalf<axFloat32>() { return 0.5f; }
template<> AX_INLINE axFloat64 axColorElementUtil::kHalf<axFloat64>() { return 0.5; }



//--- axFloat64 ---
template<> AX_INLINE
axFloat64	axColorElementUtil::to<axFloat64, axUNorm8>(axUNorm8 v) {
	return v.value / 255.0;
}
template<> AX_INLINE
axFloat64	axColorElementUtil::to<axFloat64, axUNorm16>(axUNorm16 v) {
	return v.value / 65535.0;
}
template<> AX_INLINE
axFloat64	axColorElementUtil::to<axFloat64, axFloat16>(axFloat16 v) {
	return v.toFloat32();
}
template<> AX_INLINE
axFloat64	axColorElementUtil::to<axFloat64, axFloat32>(axFloat32 v) {
	return v;
}
template<> AX_INLINE
axFloat64	axColorElementUtil::to<axFloat64, axFloat64>(axFloat64 v) {
	return v;
}

//--- axFloat32 ---
template<> AX_INLINE
axFloat32	axColorElementUtil::to<axFloat32, axUNorm8>(axUNorm8 v) {
	return v.value / 255.0f;
}
template<> AX_INLINE
axFloat32	axColorElementUtil::to<axFloat32, axUNorm16>(axUNorm16 v) {
	return v.value / 65535.0f;
}
template<> AX_INLINE
axFloat32	axColorElementUtil::to<axFloat32, axFloat16>(axFloat16 v) {
	return v.toFloat32();
}
template<> AX_INLINE
axFloat32	axColorElementUtil::to<axFloat32, axFloat32>(axFloat32 v) {
	return v;
}
template<> AX_INLINE
axFloat32	axColorElementUtil::to<axFloat32, axFloat64>(axFloat64 v) {
	return static_cast<axFloat32>(v);
}

//--- axFloat16 ---
template<> AX_INLINE
axFloat16	axColorElementUtil::to<axFloat16, axUNorm8>(axUNorm8 v) {
	return axFloat16_make(v.value / 255.0f);
}
template<> AX_INLINE
axFloat16	axColorElementUtil::to<axFloat16, axUNorm16>(axUNorm16 v) {
	return axFloat16_make(v.value / 65535.0f);
}
template<> AX_INLINE
axFloat16	axColorElementUtil::to<axFloat16, axFloat16 >(axFloat16 v) {
	return v;
}
template<> AX_INLINE
axFloat16	axColorElementUtil::to<axFloat16, axFloat32>(axFloat32 v) {
	return axFloat16_make(v);
}
template<> AX_INLINE
axFloat16	axColorElementUtil::to<axFloat16, axFloat64>(axFloat64 v) {
	return axFloat16_make(v);
}

//--- axUNorm16 ---
template<> AX_INLINE
axUNorm16	axColorElementUtil::to<axUNorm16, axUNorm8>(axUNorm8  v) {
	return static_cast<axUInt16>(static_cast<axUInt16>(v.value) << 8);
}
template<> AX_INLINE
axUNorm16	axColorElementUtil::to<axUNorm16, axUNorm16>(axUNorm16  v) {
	return v;
}
template<> AX_INLINE
axUNorm16	axColorElementUtil::to<axUNorm16, axFloat16>(axFloat16 v) {
	return static_cast<axUInt16>(ax_clamp(v.toFloat32() * 65535.0f, 0.0f, 65535.0f));
}
template<> AX_INLINE
axUNorm16	axColorElementUtil::to<axUNorm16, axFloat32>(axFloat32 v) {
	return static_cast<axUInt16>(ax_clamp(v * 65535.0f, 0.0f, 65535.0f));
}
template<> AX_INLINE
axUNorm16	axColorElementUtil::to<axUNorm16, axFloat64>(axFloat64 v) {
	return static_cast<axUInt16>(ax_clamp(v * 65535.0, 0.0, 65535.0));
}

//--- axUNorm8 ---
template<> AX_INLINE
axUNorm8	axColorElementUtil::to<axUNorm8, axUNorm8>(axUNorm8  v) {
	return v;
}
template<> AX_INLINE
axUNorm8	axColorElementUtil::to<axUNorm8, axUNorm16>(axUNorm16  v) {
	return static_cast<axUInt8>((v.value + 0x80) >> 8);
}
template<> AX_INLINE
axUNorm8	axColorElementUtil::to<axUNorm8, axFloat16>(axFloat16 v) {
	return static_cast<axUInt8>(ax_clamp(v.toFloat32() * 255.0f, 0.0f, 255.0f));
}
template<> AX_INLINE
axUNorm8	axColorElementUtil::to<axUNorm8, axFloat32>(axFloat32 v) {
	return static_cast<axUInt8>(ax_clamp(v * 255.0f, 0.0f, 255.0f));
}
template<> AX_INLINE
axUNorm8	axColorElementUtil::to<axUNorm8, axFloat64>(axFloat64 v) {
	return static_cast<axUInt8>(ax_clamp(v * 255.0, 0.0, 255.0));
}

//--------------------

template<> AX_INLINE
axFloat64	axColorElementUtil::grayscale<axFloat64>(axFloat64 r, axFloat64 g, axFloat64 b) {
	return r * 0.299 + g * 0.587 + b * 0.114f;
}
template<> AX_INLINE
axFloat32	axColorElementUtil::grayscale<axFloat32>(axFloat32 r, axFloat32 g, axFloat32 b) {
	return r * 0.299f + g * 0.587f + b * 0.114f;
}
template<> AX_INLINE
axFloat16	axColorElementUtil::grayscale<axFloat16>(axFloat16 r, axFloat16 g, axFloat16 b) {
	return to<axFloat16>(grayscale(toDouble(r), toDouble(g), toDouble(b)));
}
template<> AX_INLINE
axUNorm16	axColorElementUtil::grayscale<axUNorm16>(axUNorm16 r, axUNorm16 g, axUNorm16 b) {
	axUInt64 o 	= static_cast<axUInt64>(r.value) * 19595
				+ static_cast<axUInt64>(g.value) * 38469
				+ static_cast<axUInt64>(b.value) * 7471;
	return static_cast<axUInt16>(o / 65536);
}
template<> AX_INLINE
axUNorm8	axColorElementUtil::grayscale<axUNorm8>(axUNorm8 r, axUNorm8 g, axUNorm8 b) {
	axUInt32 o 	= static_cast<axUInt32>(r.value) * 76
				+ static_cast<axUInt32>(g.value) * 150
				+ static_cast<axUInt32>(b.value) * 29;
	return static_cast<axUInt8>(o / 255);
}
