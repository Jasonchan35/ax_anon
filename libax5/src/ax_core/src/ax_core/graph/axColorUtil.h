#pragma once

#include "axColorRGBA.h"
#include "axColorLA.h"
#include "axColorHSBA.h"
#include "axColorBGRA.h"
#include "axColorR5G6B5.h"
#include "axColorR5G5B5A1.h"
#include "axColorCompressed.h"

#define AX_MACRO_OP(T, VALUE, SUFFIX, ...) \
	template<class SRC> AX_INLINE axColorRGBA ##SUFFIX axColorRGBA ##SUFFIX## _make(const SRC& src) { return axColorRGBA ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorRGB  ##SUFFIX axColorRGB  ##SUFFIX## _make(const SRC& src) { return axColorRGB  ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorRG   ##SUFFIX axColorRG   ##SUFFIX## _make(const SRC& src) { return axColorRG   ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorR    ##SUFFIX axColorR    ##SUFFIX## _make(const SRC& src) { return axColorR    ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorLA   ##SUFFIX axColorLA   ##SUFFIX## _make(const SRC& src) { return axColorLA   ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorL    ##SUFFIX axColorL    ##SUFFIX## _make(const SRC& src) { return axColorL    ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorA    ##SUFFIX axColorA    ##SUFFIX## _make(const SRC& src) { return axColorA    ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorHSBA ##SUFFIX axColorHSBA ##SUFFIX## _make(const SRC& src) { return axColorHSBA ##SUFFIX::s_make(src); } \
	template<class SRC> AX_INLINE axColorBGRA ##SUFFIX axColorBGRA ##SUFFIX## _make(const SRC& src) { return axColorBGRA ##SUFFIX::s_make(src); } \
//---
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP

AX_INLINE axColorRGBAh axColorRGBAh_make(float r, float g, float b, float a = 1.0f) { return axColorRGBAh_make(axColorRGBAf(r,g,b,a)); }
AX_INLINE axVec4f axVec4f_make(const axColorRGBAf& c) { return axVec4f(c.r, c.g, c.b, c.a); }
AX_INLINE axVec3f axVec3f_make(const axColorRGBf&  c) { return axVec3f(c.r, c.g, c.b); }

struct axColorUtil {
	template<class DST, class SRC>
	static void convertSpan(axMutSpan<DST> dstBuffer, axSpan<SRC> srcBuffer);

	static void convertSpanByType(	axColorType dstType, axByteMutSpan dstBuffer, 
									axColorType srcType, axByteSpan    srcBuffer);

	static axInt pixelSizeInBytes(axColorType type);
	static axInt alphaBits(axColorType type);

	static const axType* elementType(axColorType type);
	static axColorModel colorModel(axColorType type);

	static axInt bytesPerPixelBlock(axColorType type);
	static bool isCompressedType(axColorType type) { return colorModel(type) == axColorModel::Compressed; }
private:
	axColorUtil() = delete;
};

template<class DST, class SRC> inline
void axColorUtil::convertSpan(axMutSpan<DST> dstBuffer, axSpan<SRC> srcBuffer) {
	if (dstBuffer.size() < srcBuffer.size()) {
		throw axError_Undefined(AX_LOC);
	}

	auto* dst = dstBuffer.begin();
	auto* src = srcBuffer.begin();
	auto* end = srcBuffer.end();

	while (src < end) {
		*dst = DST::s_make(*src);
		src++;
		dst++;
	}
}
