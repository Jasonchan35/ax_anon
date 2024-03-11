#include "axColor.h"
#include "../rtti/axTypeManager.h"
#include "axColorUtil.h"

void axColorUtil::convertSpanByType(axColorType dstType, axByteMutSpan dstBuffer, 
									axColorType srcType, axByteSpan    srcBuffer) 
{
	if (dstType == srcType) {
		if (dstBuffer.size() < srcBuffer.size()) {
			throw axError_Undefined(AX_LOC);
		}
		axArrayUtil::copy(dstBuffer.data(), srcBuffer.data(), srcBuffer.sizeInBytes());
		return;
	}

#define AX_CONVERT_SPAN_CASE(DST, SRC) \
	case SRC::kColorType: { \
		return convertSpan<DST, SRC>( \
			axMutSpan<DST>::s_fromByteSpan(dstBuffer), \
			   axSpan<SRC>::s_fromByteSpan(srcBuffer)  \
		); \
	}break; \
//----
	
	switch (dstType) {
		case axColorType::Lb: {
			switch (srcType) {
				AX_CONVERT_SPAN_CASE(axColorLb, axColorLb);
				AX_CONVERT_SPAN_CASE(axColorLb, axColorRGBb);
				AX_CONVERT_SPAN_CASE(axColorLb, axColorRGBAb);
			}break;
		}break;

		case axColorType::RGBAb: {
			switch (srcType) {
				AX_CONVERT_SPAN_CASE(axColorRGBAb, axColorLb);
				AX_CONVERT_SPAN_CASE(axColorRGBAb, axColorRGBb);
				AX_CONVERT_SPAN_CASE(axColorRGBAb, axColorRGBAb);
			}break;
		}break;
	}

#undef AX_CONVERT_SPAN_CASE
	throw axError_Undefined(AX_LOC);
}

axInt axColorUtil::alphaBits(axColorType type) {
	switch (type) {
		#define AX_MACRO_OP(T, ...) \
			case axColorType::T: return axColor##T::kAlphaBits; \
		//----------
			axColorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
	}
	return 0;
}

axInt axColorUtil::pixelSizeInBytes(axColorType type) {
	switch (type) {
		#define AX_MACRO_OP(T, ...) \
			case axColorType::T: return ax_sizeof(axColor##T);
		//----------
			axColorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
		default: return 0;
	}
}

const axType* axColorUtil::elementType(axColorType type) {
	switch (type) {
		#define AX_MACRO_OP(T, ...) \
			case axColorType::T: return ax_typeof<axColor##T::Element>();
		//----------
			axColorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
		default: return nullptr;
	}
}

axColorModel axColorUtil::colorModel(axColorType type) {
	switch (type) {
		#define AX_MACRO_OP(T, ...) \
			case axColorType::T: return axColor##T::kColorModel;
		//----------
			axColorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
		default: return axColorModel::Unknown;
	}
}

axInt axColorUtil::bytesPerPixelBlock(axColorType type) {
	switch (type) {
		#define AX_MACRO_OP(T, ...) \
			case axColorType::T: return axColor##T::kBytesPerPixelBlock;
		//----------
			axColorType_Compressed_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
		default: return 0;
	}
	return axInt();
}
