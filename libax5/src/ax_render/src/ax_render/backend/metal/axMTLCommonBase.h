#pragma once

#if AX_USE_METAL

#define ax_Metal_TODO

#import <MetalKit/MetalKit.h>

#include "../../vertex/axRenderVertexDesc.h"
#include "../../texture/axSamplerState.h"
#include "../../axRenderCommonBase.h"

class axMTLRenderer;
class axMTLRenderContext;
class axMTLCommandDispatcher;
class axMTLTexture2D;

class axMTLUtil : public axRenderCommonBase {
public:
	

	enum class BufferIndexUsage : NSUInteger {
		// UniformBuffer / StorageBuffer 0 ~ 15
		VertexAttr = 16,
	};
	
	static axMTLRenderer*		renderer();
	static id<MTLDevice>		mtlDevice();

	static MTLPrimitiveType				getMTLPrimitiveType			(axRenderPrimitiveType t);
	static MTLPrimitiveTopologyClass	getMTLPrimitiveTopologyClass(axRenderPrimitiveType t);
	static MTLIndexType					getMTLIndexType				(axRenderIndexType t);

	static MTLCompareFunction			getMTLDepthTestOp			(axRenderState_DepthTestOp	v);
	static MTLBlendOperation			getMTLBlendOp				(axRenderState_BlendOp		v);
	static MTLBlendFactor				getMTLBlendFactor			(axRenderState_BlendFactor	v);
	static MTLCullMode					getMTLCullMode				(axRenderState_Cull v);
	static MTLVertexFormat				getMTLVertexFormat			(axRenderDataType v);
	static MTLPixelFormat				getMTLPixelFormat			(axColorType v);

	static MTLSamplerMinMagFilter		getMTLTextureMinMagFilter	(axTextureFilter v);
	static MTLSamplerMipFilter			getMTLTextureMipFilter		(axTextureFilter v, axInt mipmapCount);
	static MTLSamplerAddressMode		getMTLTextureWrap			(axTextureWrap   v);

	static axRenderDataType				getAxDataType		(MTLDataType t);
	static axRenderDataType				getAxDataType		(MTLTextureType t);

	static NSInteger	castNSInt (axInt v) { return ax_safe_static_cast<NSInteger >(v); }
	static NSUInteger	castNSUInt(axInt v) { return ax_safe_static_cast<NSUInteger>(v); }
};


inline
MTLPrimitiveType axMTLUtil::getMTLPrimitiveType(axRenderPrimitiveType t) {
	using SRC = axRenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return MTLPrimitiveTypePoint;
		case SRC::Lines:		return MTLPrimitiveTypeLine;
		case SRC::Triangles:	return MTLPrimitiveTypeTriangle;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLPrimitiveTopologyClass axMTLUtil::getMTLPrimitiveTopologyClass(axRenderPrimitiveType t) {
	using SRC = axRenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return MTLPrimitiveTopologyClassPoint;
		case SRC::Lines:		return MTLPrimitiveTopologyClassLine;
		case SRC::Triangles:	return MTLPrimitiveTopologyClassTriangle;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLIndexType axMTLUtil::getMTLIndexType(axRenderIndexType t) {
	using SRC = axRenderIndexType;
	switch (t) {
		case SRC::UInt16:	return MTLIndexTypeUInt16;
		case SRC::UInt32:	return MTLIndexTypeUInt32;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
axRenderDataType axMTLUtil::getAxDataType(MTLDataType t) {
	using DST = axRenderDataType;
	switch (t) {
		case MTLDataTypeInt:		return DST::Int32;
		case MTLDataTypeInt2:		return DST::Int32x2;
		case MTLDataTypeInt3:		return DST::Int32x3;
		case MTLDataTypeInt4:		return DST::Int32x4;
	//---
		case MTLDataTypeUInt:		return DST::UInt32;
		case MTLDataTypeUInt2:		return DST::UInt32x2;
		case MTLDataTypeUInt3:		return DST::UInt32x3;
		case MTLDataTypeUInt4:		return DST::UInt32x4;
	//---
		case MTLDataTypeFloat:		return DST::Float32;
		case MTLDataTypeFloat2:		return DST::Float32x2;
		case MTLDataTypeFloat3:		return DST::Float32x3;
		case MTLDataTypeFloat4:		return DST::Float32x4;
	//---
		case MTLDataTypeFloat2x2:	return DST::Float32_2x2;
		case MTLDataTypeFloat3x3:	return DST::Float32_3x3;
		case MTLDataTypeFloat4x4:	return DST::Float32_4x4;
		case MTLDataTypeFloat4x3:	return DST::Float32_4x3;
	//---
		case MTLDataTypeSampler:	return DST::SamplerState;
	//---
		case MTLDataTypeArray:		return DST::StorageBuffer;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
axRenderDataType axMTLUtil::getAxDataType(MTLTextureType v) {
	using DST = axRenderDataType;
	switch (v) {
		case MTLTextureType1D:			return DST::Texture1D;
		case MTLTextureType2D:			return DST::Texture2D;
		case MTLTextureType3D:			return DST::Texture3D;
		case MTLTextureTypeCube:		return DST::TextureCube;
	//---
		case MTLTextureType1DArray:		return DST::Texture1DArray;
		case MTLTextureType2DArray:		return DST::Texture2DArray;
//		case MTLTextureType3DArray:		return DST::Texture3DArray;
		case MTLTextureTypeCubeArray:	return DST::TextureCubeArray;
	//----
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLCompareFunction axMTLUtil::getMTLDepthTestOp(axRenderState_DepthTestOp v) {
	using SRC = axRenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return MTLCompareFunctionAlways;
		case SRC::Less:			return MTLCompareFunctionLess;
		case SRC::Equal:		return MTLCompareFunctionEqual;
		case SRC::LessEqual:	return MTLCompareFunctionLessEqual;
		case SRC::Greater:		return MTLCompareFunctionGreater;
		case SRC::GreaterEqual:	return MTLCompareFunctionGreaterEqual;
		case SRC::NotEqual:		return MTLCompareFunctionNotEqual;
		case SRC::Never:		return MTLCompareFunctionNever;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLBlendOperation axMTLUtil::getMTLBlendOp(axRenderState_BlendOp		v) {
	using SRC = axRenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return MTLBlendOperationAdd;
		case SRC::Sub:		return MTLBlendOperationSubtract;
		case SRC::RevSub:	return MTLBlendOperationReverseSubtract;
		case SRC::Min:		return MTLBlendOperationMin;
		case SRC::Max:		return MTLBlendOperationMax;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLBlendFactor axMTLUtil::getMTLBlendFactor(axRenderState_BlendFactor v) {
	using SRC = axRenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:						return MTLBlendFactorZero;
		case SRC::One:						return MTLBlendFactorOne;
	//---
		case SRC::SrcAlpha:					return MTLBlendFactorSourceAlpha;
		case SRC::DstAlpha:					return MTLBlendFactorDestinationAlpha;
		case SRC::SrcColor:					return MTLBlendFactorSourceColor;
		case SRC::DstColor:					return MTLBlendFactorDestinationColor;
		case SRC::ConstColor:				return MTLBlendFactorBlendColor;
//		case SRC::ConstAlpha:				return MTLBlendFactorBlendAlpha;
	//---
		case SRC::OneMinusSrcAlpha:			return MTLBlendFactorOneMinusSourceAlpha;
		case SRC::OneMinusSrcColor:			return MTLBlendFactorOneMinusSourceColor;
		case SRC::OneMinusDstAlpha:			return MTLBlendFactorOneMinusDestinationAlpha;
		case SRC::OneMinusDstColor:			return MTLBlendFactorOneMinusDestinationColor;
		case SRC::OneMinusConstColor:		return MTLBlendFactorOneMinusBlendColor;
//		case SRC::OneMinusConstAlpha:		return MTLBlendFactorOneMinusBlendAlpha;
	//---
		case SRC::SrcAlphaSaturate:			return MTLBlendFactorSourceAlphaSaturated;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLCullMode axMTLUtil::getMTLCullMode(axRenderState_Cull v) {
	using SRC = axRenderState_Cull;
	switch (v) {
		case SRC::Back:		return MTLCullModeBack;
		case SRC::Front:	return MTLCullModeFront;
		case SRC::None:		return MTLCullModeNone;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLVertexFormat axMTLUtil::getMTLVertexFormat(axRenderDataType v) {
	using SRC = axRenderDataType;
	switch (v) {
	//	case SRC::UInt8:		return MTLVertexFormatUChar;// require iOS 10.13
		case SRC::UInt8x2:		return MTLVertexFormatUChar2Normalized;
		case SRC::UInt8x3:		return MTLVertexFormatUChar3Normalized;
		case SRC::UInt8x4:		return MTLVertexFormatUChar4Normalized;
	//---
	//	case SRC::Float16:		return MTLVertexFormatHalf; break;// require iOS 10.13
		case SRC::Float16x2:	return MTLVertexFormatHalf2;
		case SRC::Float16x3:	return MTLVertexFormatHalf3;
		case SRC::Float16x4:	return MTLVertexFormatHalf4;
	//---
		case SRC::Float32:		return MTLVertexFormatFloat;
		case SRC::Float32x2:	return MTLVertexFormatFloat2;
		case SRC::Float32x3:	return MTLVertexFormatFloat3;
		case SRC::Float32x4:	return MTLVertexFormatFloat4;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLPixelFormat axMTLUtil::getMTLPixelFormat(axColorType v) {
	using SRC = axColorType;
	switch (v) {
	//---
		case SRC::RGBAb: return MTLPixelFormatRGBA8Unorm;
//		case SRC::RGBb:
		case SRC::RGb:	 return MTLPixelFormatRG8Unorm;
		case SRC::Rb:	 return MTLPixelFormatR8Unorm;
		case SRC::Ab:	 return MTLPixelFormatA8Unorm;
		case SRC::Lb:	 return MTLPixelFormatR8Unorm;
		case SRC::LAb:	 return MTLPixelFormatRG8Unorm;
	//---
		case SRC::RGBAh: return MTLPixelFormatRGBA16Float;
	//---
		case SRC::HSBAf: return MTLPixelFormatRGBA32Float;
		case SRC::RGBAf: return MTLPixelFormatRGBA32Float;
		case SRC::Af:	 return MTLPixelFormatR32Float;
		case SRC::Lf:	 return MTLPixelFormatR32Float;
		case SRC::LAf:	 return MTLPixelFormatRG32Float;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLSamplerMinMagFilter axMTLUtil::getMTLTextureMinMagFilter(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return MTLSamplerMinMagFilterNearest;
		case SRC::Linear:		return MTLSamplerMinMagFilterNearest;
		case SRC::Bilinear:		return MTLSamplerMinMagFilterLinear;
		case SRC::Trilinear:	return MTLSamplerMinMagFilterLinear;
		case SRC::Anisotropic:	return MTLSamplerMinMagFilterLinear;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLSamplerMipFilter	axMTLUtil::getMTLTextureMipFilter(axTextureFilter v, axInt mipmapCount) {
	if (mipmapCount <= 1)
		return MTLSamplerMipFilterNotMipmapped;

	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return MTLSamplerMipFilterNearest;
		case SRC::Linear:		return MTLSamplerMipFilterLinear;
		case SRC::Bilinear:		return MTLSamplerMipFilterNearest;
		case SRC::Trilinear:	return MTLSamplerMipFilterLinear;
		case SRC::Anisotropic:	return MTLSamplerMipFilterLinear;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
MTLSamplerAddressMode axMTLUtil::getMTLTextureWrap(axTextureWrap v) {
	using SRC = axTextureWrap;
	switch (v) {
		case SRC::Repeat:		return MTLSamplerAddressModeRepeat;
		case SRC::Clamp:		return MTLSamplerAddressModeClampToEdge;
		case SRC::Mirror:		return MTLSamplerAddressModeMirrorRepeat;
		case SRC::MirrorOnce:	return MTLSamplerAddressModeMirrorClampToEdge;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_METAL
