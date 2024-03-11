#pragma once

#include <ax_render/ax_render_config.h>

#define axRenderComponentFlags_EnumList(E) \
	E(None,) \
	E(Point,	 = 1 << 0) \
	E(Vertex,	 = 1 << 1) \
	E(Edge,		 = 1 << 2) \
	E(Primitive, = 1 << 3) \
//----
ax_ENUM_FLAGS_CLASS(axRenderComponentFlags, axUInt8)

#define axRenderPrimitiveType_EnumList(E) \
	E(Unknown,) \
	E(Points,) \
	E(Lines,) \
	E(Triangles,) \
//----
ax_ENUM_CLASS(axRenderPrimitiveType, axUInt8)

#define axRenderIndexType_EnumList(E) \
	E(Unknown,) \
	E(UInt16,) \
	E(UInt32,) \
//----
ax_ENUM_CLASS(axRenderIndexType, axUInt8)

#define axShaderStageFlags_EnumList(E) \
	E(Unknown,) \
	E(Vertex,	= 1 << 0) \
	E(Pixel,	= 1 << 1)  \
	E(Compute,	= 1 << 2) \
	E(_END,) \
//----
ax_ENUM_FLAGS_CLASS(axShaderStageFlags, axUInt32)

#define axRenderBufferType_EnumList(E) \
	E(Unknown,) \
	E(Vertex,) \
	E(Index,) \
	E(Const,) \
	E(Storage,) \
	E(Stage,) \
	E(Upload,) /* TODO: remove it ? */ \
	E(UAV,)    /* TODO: remove it ? */ \
	E(ShaderRecord,) \
	E(RayTracingScratch,) \
	E(RayTracingInstanceDesc,) \
	E(RayTracingAccelStruct,) \
//----
ax_ENUM_CLASS(axRenderBufferType, axUInt8)

#define axRenderGpuQueryType_EnumList(E) \
	E(Unknown,) \
	E(Begin,) \
	E(Event,) \
//----
ax_ENUM_CLASS(axRenderGpuQueryType, axUInt8)

#define axRenderVertexAttrId_EnumList(E) \
	E(Unknown,) \
	E(POSITION0,) \
	E(SV_POSITION0,) \
	E(TEXCOORD0,) \
	E(TEXCOORD1,) \
	E(TEXCOORD2,) \
	E(TEXCOORD3,) \
	E(TEXCOORD4,) \
	E(TEXCOORD5,) \
	E(TEXCOORD6,) \
	E(TEXCOORD7,) \
	E(TEXCOORD_END,) \
	E(COLOR0,) \
	E(COLOR1,) \
	E(COLOR2,) \
	E(COLOR3,) \
	E(COLOR4,) \
	E(COLOR5,) \
	E(COLOR6,) \
	E(COLOR7,) \
	E(COLOR_END,) \
	E(NORMAL0,) \
	E(TANGENT0,) \
	E(BINORMAL0,) \
	E(CUSTOM0,) \
	E(CUSTOM1,) \
	E(CUSTOM2,) \
	E(CUSTOM3,) \
	E(CUSTOM4,) \
	E(CUSTOM5,) \
	E(CUSTOM6,) \
	E(CUSTOM7,) \
	E(CUSTOM_END,) \
	E(_END,) \
//--------------
ax_ENUM_CLASS(axRenderVertexAttrId, axUInt8);
AX_STATIC_ASSERT(ax_enum_int(axRenderVertexAttrId::_END) < 64);

#define axRenderDataType_EnumList(E) \
	E(Unknown,) \
	\
	E(Int8,  )   E(Int8x2,)    E(Int8x3,)    E(Int8x4,) \
	E(Int16, )   E(Int16x2,)   E(Int16x3,)   E(Int16x4,) \
	E(Int32, )   E(Int32x2,)   E(Int32x3,)   E(Int32x4,) \
	E(Int64, )   E(Int64x2,)   E(Int64x3,)   E(Int64x4,) \
	\
	E(UInt8,  )   E(UInt8x2,)    E(UInt8x3,)    E(UInt8x4,) \
	E(UInt16, )   E(UInt16x2,)   E(UInt16x3,)   E(UInt16x4,) \
	E(UInt32, )   E(UInt32x2,)   E(UInt32x3,)   E(UInt32x4,) \
	E(UInt64, )   E(UInt64x2,)   E(UInt64x3,)   E(UInt64x4,) \
	\
	E(SNorm8,  )   E(SNorm8x2,)    E(SNorm8x3,)    E(SNorm8x4,) \
	E(SNorm16, )   E(SNorm16x2,)   E(SNorm16x3,)   E(SNorm16x4,) \
	E(SNorm32, )   E(SNorm32x2,)   E(SNorm32x3,)   E(SNorm32x4,) \
	\
	E(UNorm8,  )  E(UNorm8x2,)   E(UNorm8x3,)   E(UNorm8x4,) \
	E(UNorm16, )  E(UNorm16x2,)  E(UNorm16x3,)  E(UNorm16x4,) \
	E(UNorm32, )  E(UNorm32x2,)  E(UNorm32x3,)  E(UNorm32x4,) \
	\
	E(Float16, ) E(Float16x2,) E(Float16x3,) E(Float16x4,) \
	E(Float32, ) E(Float32x2,) E(Float32x3,) E(Float32x4,) \
	E(Float64, ) E(Float64x2,) E(Float64x3,) E(Float64x4,) \
	\
	E(Float16_2x2, ) E(Float16_3x3,) E(Float16_4x4,) E(Float16_4x3,) \
	E(Float32_2x2, ) E(Float32_3x3,) E(Float32_4x4,) E(Float32_4x3,) \
	E(Float64_2x2, ) E(Float64_3x3,) E(Float64_4x4,) E(Float64_4x3,) \
	\
	E(Bool,)  E(Boolx2,)  E(Boolx3,)  E(Boolx4,) \
	\
	E(SamplerState,) \
	\
	E(Texture1D,)   \
	E(Texture2D,)   \
	E(Texture3D,)   \
	E(TextureCube,) \
	\
	E(Texture1DArray,)   \
	E(Texture2DArray,)   \
	E(Texture3DArray,)   \
	E(TextureCubeArray,) \
	\
	E(StorageBuffer,) \
//-------
ax_ENUM_CLASS(axRenderDataType, axUInt8);


#define axRenderState_DepthTestOp_EnumList(E) \
	E(Less,) \
	E(Equal,) \
	E(LessEqual,) \
	E(Greater,) \
	E(GreaterEqual,) \
	E(NotEqual,) \
	E(Always,) \
	E(Never,) \
	E(_END,) \
//-----
ax_ENUM_CLASS(axRenderState_DepthTestOp, axUInt8)

#define axRenderState_BlendOp_EnumList(E) \
	E(Disable,) \
	E(Add,) \
	E(Sub,) \
	E(RevSub,) \
	E(Min,) \
	E(Max,) \
	E(_END,) \
//-----
ax_ENUM_CLASS(axRenderState_BlendOp, axUInt8)

#define axRenderState_BlendFactor_EnumList(E) \
	E(Zero,) \
	E(One,) \
	E(SrcColor,) \
	E(DstColor,) \
	E(SrcAlpha,) \
	E(DstAlpha,) \
	E(ConstColor,) \
/*	E(ConstAlpha,) */ \
	E(OneMinusSrcColor,) \
	E(OneMinusDstColor,) \
	E(OneMinusSrcAlpha,) \
	E(OneMinusDstAlpha,) \
	E(OneMinusConstColor,) \
/*	E(OneMinusConstAlpha,) */\
	E(SrcAlphaSaturate,) \
	E(_END,) \
//-----
ax_ENUM_CLASS(axRenderState_BlendFactor, axUInt8)

#define axRenderState_Cull_EnumList(E) \
	E(Back,) \
	E(Front,) \
	E(None,) \
//----
ax_ENUM_CLASS(axRenderState_Cull, axUInt8)

class axRenderDataTypeUtil {
public:
	using DataType = axRenderDataType;

	static bool		isTexture		(DataType t);
	static axInt16	elementCount	(DataType t);
	static axInt16	sizeInBytes		(DataType t);

	template<class T> 
	constexpr static DataType	typeOf();

private:
	axRenderDataTypeUtil() = delete;
};

inline bool axRenderDataTypeUtil::isTexture(DataType t) {
	switch (t) {
		case DataType::Texture1D:
		case DataType::Texture2D:
		case DataType::Texture3D:
		case DataType::TextureCube:
	//---
		case DataType::Texture1DArray:
		case DataType::Texture2DArray:
		case DataType::Texture3DArray:
		case DataType::TextureCubeArray:
			return true;
	}
	return false;
}

inline
axInt16 axRenderDataTypeUtil::elementCount(DataType t) {
	switch (t) {
		case DataType::Bool:
		case DataType::Int8:
		case DataType::Int16:
		case DataType::Int32:
		case DataType::Int64:
		case DataType::UInt8:
		case DataType::UInt16:
		case DataType::UInt32:
		case DataType::UInt64:
		case DataType::Float16:
		case DataType::Float32:
		case DataType::Float64:
			return 1;

		case DataType::Int8x2:
		case DataType::Int16x2:
		case DataType::Int32x2:
		case DataType::Int64x2:
		case DataType::UInt8x2:
		case DataType::UInt16x2:
		case DataType::UInt32x2:
		case DataType::UInt64x2:
		case DataType::Float16x2:
		case DataType::Float32x2:
		case DataType::Float64x2:
			return 2;

		case DataType::Int8x3:
		case DataType::Int16x3:
		case DataType::Int32x3:
		case DataType::Int64x3:
		case DataType::UInt8x3:
		case DataType::UInt16x3:
		case DataType::UInt32x3:
		case DataType::UInt64x3:
		case DataType::Float16x3:
		case DataType::Float32x3:
		case DataType::Float64x3:
			return 3;

		case DataType::Int8x4:
		case DataType::Int16x4:
		case DataType::Int32x4:
		case DataType::Int64x4:
		case DataType::UInt8x4:
		case DataType::UInt16x4:
		case DataType::UInt32x4:
		case DataType::UInt64x4:
		case DataType::Float16x4:
		case DataType::Float32x4:
		case DataType::Float64x4:
			return 4;
	//---
		default: {
			AX_LOG("Unhandled type = {?}", t);
			throw axError_Undefined(AX_LOC);
		}
	}
}

inline axInt16 axRenderDataTypeUtil::sizeInBytes(DataType t) {
	switch (t) {
		case DataType::Int8:		return ax_sizeof(axInt8);
		case DataType::Int8x2:		return ax_sizeof(axInt8) * 2;
		case DataType::Int8x3:		return ax_sizeof(axInt8) * 3;
		case DataType::Int8x4:		return ax_sizeof(axInt8) * 4;
	//---
		case DataType::UInt8:		return ax_sizeof(axUInt8);
		case DataType::UInt8x2:		return ax_sizeof(axUInt8) * 2;
		case DataType::UInt8x3:		return ax_sizeof(axUInt8) * 3;
		case DataType::UInt8x4:		return ax_sizeof(axUInt8) * 4;
	//---
		case DataType::Int16:		return ax_sizeof(axInt16);
		case DataType::Int16x2:		return ax_sizeof(axInt16) * 2;
		case DataType::Int16x3:		return ax_sizeof(axInt16) * 3;
		case DataType::Int16x4:		return ax_sizeof(axInt16) * 4;
	//---
		case DataType::UInt16:		return ax_sizeof(axUInt16);
		case DataType::UInt16x2:	return ax_sizeof(axUInt16) * 2;
		case DataType::UInt16x3:	return ax_sizeof(axUInt16) * 3;
		case DataType::UInt16x4:	return ax_sizeof(axUInt16) * 4;
	//---
		case DataType::Int32:		return ax_sizeof(axInt32);
		case DataType::Int32x2:		return ax_sizeof(axInt32) * 2;
		case DataType::Int32x3:		return ax_sizeof(axInt32) * 3;
		case DataType::Int32x4:		return ax_sizeof(axInt32) * 4;
	//---
		case DataType::UInt32:		return ax_sizeof(axUInt32);
		case DataType::UInt32x2:	return ax_sizeof(axUInt32) * 2;
		case DataType::UInt32x3:	return ax_sizeof(axUInt32) * 3;
		case DataType::UInt32x4:	return ax_sizeof(axUInt32) * 4;
	//---
		case DataType::Int64:		return ax_sizeof(axInt64);
		case DataType::Int64x2:		return ax_sizeof(axInt64) * 2;
		case DataType::Int64x3:		return ax_sizeof(axInt64) * 3;
		case DataType::Int64x4:		return ax_sizeof(axInt64) * 4;
	//---
		case DataType::UInt64:		return ax_sizeof(axUInt64);
		case DataType::UInt64x2:	return ax_sizeof(axUInt64) * 2;
		case DataType::UInt64x3:	return ax_sizeof(axUInt64) * 3;
		case DataType::UInt64x4:	return ax_sizeof(axUInt64) * 4;
	//---
		case DataType::Float16:		return ax_sizeof(axFloat16);
		case DataType::Float16x2:	return ax_sizeof(axFloat16) * 2;
		case DataType::Float16x3:	return ax_sizeof(axFloat16) * 3;
		case DataType::Float16x4:	return ax_sizeof(axFloat16) * 4;
	//---
		case DataType::Float32:		return ax_sizeof(axFloat32);
		case DataType::Float32x2:	return ax_sizeof(axFloat32) * 2;
		case DataType::Float32x3:	return ax_sizeof(axFloat32) * 3;
		case DataType::Float32x4:	return ax_sizeof(axFloat32) * 4;
	//---
		case DataType::Float64:		return ax_sizeof(axFloat64);
		case DataType::Float64x2:	return ax_sizeof(axFloat64) * 2;
		case DataType::Float64x3:	return ax_sizeof(axFloat64) * 3;
		case DataType::Float64x4:	return ax_sizeof(axFloat64) * 4;
	//---
		case DataType::Float16_2x2: return ax_sizeof(axFloat16) * 2 * 2;
		case DataType::Float16_3x3: return ax_sizeof(axFloat16) * 3 * 3;
		case DataType::Float16_4x4: return ax_sizeof(axFloat16) * 4 * 4;
		case DataType::Float16_4x3: return ax_sizeof(axFloat16) * 4 * 3;
	//---
		case DataType::Float32_2x2: return ax_sizeof(axFloat32) * 2 * 2;
		case DataType::Float32_3x3: return ax_sizeof(axFloat32) * 3 * 3;
		case DataType::Float32_4x4: return ax_sizeof(axFloat32) * 4 * 4;
		case DataType::Float32_4x3: return ax_sizeof(axFloat32) * 4 * 3;
	//---
		case DataType::Float64_2x2: return ax_sizeof(axFloat64) * 2 * 2;
		case DataType::Float64_3x3: return ax_sizeof(axFloat64) * 3 * 3;
		case DataType::Float64_4x4: return ax_sizeof(axFloat64) * 4 * 4;
		case DataType::Float64_4x3: return ax_sizeof(axFloat64) * 4 * 3;
	//---
		case DataType::SamplerState:	return 0;
		case DataType::Texture1D:		return 0;
		case DataType::Texture2D:		return 0;
		case DataType::Texture3D:		return 0;
		case DataType::TextureCube:		return 0;
		case DataType::StorageBuffer:	return 0;
	//---
		default: {
			AX_LOG("Unhandled type = {?}", t);
			throw axError_Undefined(AX_LOC);
		}
	}
}

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<void>()			{ return DataType::Unknown; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt8>()		{ return DataType::Int8; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt8x2>()		{ return DataType::Int8x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt8x3>()		{ return DataType::Int8x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt8x4>()		{ return DataType::Int8x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt16>()		{ return DataType::Int16; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt16x2>()		{ return DataType::Int16x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt16x3>()		{ return DataType::Int16x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt16x4>()		{ return DataType::Int16x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt32>()		{ return DataType::Int32; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt32x2>()		{ return DataType::Int32x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt32x3>()		{ return DataType::Int32x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt32x4>()		{ return DataType::Int32x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt64>()		{ return DataType::Int64; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt64x2>()		{ return DataType::Int64x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt64x3>()		{ return DataType::Int64x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axInt64x4>()		{ return DataType::Int64x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt8>()		{ return DataType::UInt8; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt8x2>()		{ return DataType::UInt8x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt8x3>()		{ return DataType::UInt8x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt8x4>()		{ return DataType::UInt8x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt16>()		{ return DataType::UInt16; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt16x2>()	{ return DataType::UInt16x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt16x3>()	{ return DataType::UInt16x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt16x4>()	{ return DataType::UInt16x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt32>()		{ return DataType::UInt32; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt32x2>()	{ return DataType::UInt32x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt32x3>()	{ return DataType::UInt32x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt32x4>()	{ return DataType::UInt32x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt64>()		{ return DataType::UInt64; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt64x2>()	{ return DataType::UInt64x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt64x3>()	{ return DataType::UInt64x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axUInt64x4>()	{ return DataType::UInt64x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat16>()		{ return DataType::Float16; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat16x2>()	{ return DataType::Float16x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat16x3>()	{ return DataType::Float16x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat16x4>()	{ return DataType::Float16x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat32>()		{ return DataType::Float32; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat32x2>()	{ return DataType::Float32x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat32x3>()	{ return DataType::Float32x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat32x4>()	{ return DataType::Float32x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat64>()		{ return DataType::Float64; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat64x2>()	{ return DataType::Float64x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat64x3>()	{ return DataType::Float64x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axFloat64x4>()	{ return DataType::Float64x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec2f>()		{ return DataType::Float32x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec3f>()		{ return DataType::Float32x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec4f>()		{ return DataType::Float32x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec2d>()		{ return DataType::Float64x2; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec3d>()		{ return DataType::Float64x3; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axVec4d>()		{ return DataType::Float64x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axMat4f>()		{ return DataType::Float32_4x4; }

template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axColorRGBAb>()	{ return DataType::UInt8x4; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axColorRGBAh>()	{ return DataType::Float16x4; }
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axColorRGBAf>()	{ return DataType::Float32x4; }

class axTexture2D;
template<> constexpr axRenderDataType axRenderDataTypeUtil::typeOf<axTexture2D>()	{ return DataType::Texture2D; }
