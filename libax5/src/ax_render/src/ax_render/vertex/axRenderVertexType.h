#pragma once

#include <ax_render/axRenderDataType.h>

static constexpr axInt axRenderVertex_kAlignment = 4;

const axInt axRenderVertexAttrId_kUvCount     = ax_enum_int(axRenderVertexAttrId::TEXCOORD_END - axRenderVertexAttrId::TEXCOORD0);
const axInt axRenderVertexAttrId_kColorCount  = ax_enum_int(axRenderVertexAttrId::COLOR_END    - axRenderVertexAttrId::COLOR0);
const axInt axRenderVertexAttrId_kCustomCount = ax_enum_int(axRenderVertexAttrId::CUSTOM_END   - axRenderVertexAttrId::CUSTOM0);

enum class axRenderVertexAttrMask : axUInt64 {
	None = 0,
};

AX_ENUM_OPERATORS(axRenderVertexAttrMask)

#define axRenderNormalType_EnumList(E) \
	E(None,) \
	E(Normal,) \
	E(NormalTangent,) \
	E(NormalTangentBinormal,) \
//----
ax_ENUM_FLAGS_CLASS(axRenderNormalType, axUInt8)

enum class axRenderVertexType : axUInt64;

constexpr axRenderVertexType axRenderVertexType_make(
	axRenderDataType pos, 
	axRenderDataType uv,		axInt uvCount,
	axRenderDataType color,		axInt colorCount,
	axRenderDataType normal,	axRenderNormalType normalType,
	axRenderDataType custom,	axInt customCount)
{
	auto bit = 0;
	auto v = axUInt64(0);
	
	v |= (static_cast<axUInt64>(ax_enum_int(pos))			<< bit);	bit += 8;
	v |= (static_cast<axUInt64>(ax_enum_int(uv))			<< bit);	bit += 8;
	v |= (static_cast<axUInt64>(ax_enum_int(color))			<< bit);	bit += 8;
	v |= (static_cast<axUInt64>(ax_enum_int(normal))		<< bit);	bit += 8;
	v |= (static_cast<axUInt64>(ax_enum_int(custom))		<< bit);	bit += 8;

	v |= (static_cast<axUInt64>(colorCount)					<< bit);	bit += 3;
	v |= (static_cast<axUInt64>(uvCount)					<< bit);	bit += 3;
	v |= (static_cast<axUInt64>(ax_enum_int(normalType))	<< bit);	bit += 2;
	v |= (static_cast<axUInt64>(customCount					<< bit));	bit += 3;

	return static_cast<axRenderVertexType>(v);
}

class axRenderVertexTypeBreakdown {
public:
	using DataType		= axRenderDataType;
	using NormalType	= axRenderNormalType;

	static axRenderVertexTypeBreakdown s_make(axRenderVertexType type) {
		axRenderVertexTypeBreakdown o;
		auto bit = 0;
		auto v = ax_enum_int(type);
		o.pos			= DataType((v >> bit) & 0xFF);		bit += 8;
		o.uv			= DataType((v >> bit) & 0xFF);		bit += 8;
		o.color			= DataType((v >> bit) & 0xFF);		bit += 8;
		o.normal		= DataType((v >> bit) & 0xFF);		bit += 8;
		o.custom		= DataType((v >> bit) & 0xFF);		bit += 8;

		o.colorCount	= ((v >> bit) & ((1 << 3) - 1));	bit += 3;
		o.uvCount		= ((v >> bit) & ((1 << 3) - 1));	bit += 3;
		o.normalType	= ((v >> bit) & ((1 << 2) - 1));	bit += 2;
		o.customCount	= ((v >> bit) & ((1 << 3) - 1));	bit += 3;
		return o;
	}

	axRenderVertexType toType() const {
		return axRenderVertexType_make(pos, color, colorCount, uv, uvCount, normal, NormalType(normalType), custom, customCount);
	}

	DataType	pos;
	DataType	color;
	DataType	uv;
	DataType	normal;
	DataType	custom;

	axUInt16	colorCount	: 3;
	axUInt16	uvCount		: 3;
	axUInt16	normalType	: 2;
	axUInt16	customCount	: 3;
};

AX_STATIC_ASSERT(sizeof(axRenderVertexTypeBreakdown) == sizeof(axRenderVertexType));


template<class POS
	, class UV      = void, axInt UV_COUNT    = 0
	, class COLOR   = void, axInt COLOR_COUNT = 0
	, class NORMAL  = void, axRenderNormalType NORMAL_TYPE = axRenderNormalType::None
	, class CUSTOM  = void, axInt CUSTOM_COUNT = 0
> constexpr
axRenderVertexType axRenderVertexType_makeT() {
	return axRenderVertexType_make(
				axRenderDataTypeUtil::typeOf<POS>(),
				axRenderDataTypeUtil::typeOf<UV>(),			UV_COUNT,
				axRenderDataTypeUtil::typeOf<COLOR>(),		COLOR_COUNT,
				axRenderDataTypeUtil::typeOf<NORMAL>(),		NORMAL_TYPE,
				axRenderDataTypeUtil::typeOf<CUSTOM>(),		CUSTOM_COUNT);
};

#define axRenderVertexType_EnumList(E) \
	E(Unknown                  , = (axUInt64)(axRenderVertexType_makeT<void>())) \
	E(Pos                      , = (axUInt64)(axRenderVertexType_makeT<axFloat32x3>())) \
	E(PosColor1                , = (axUInt64)(axRenderVertexType_makeT<axFloat32x3, axColorRGBAh, 1>())) \
	E(PosColor1Normal          , = (axUInt64)(axRenderVertexType_makeT<axFloat32x3, axColorRGBAh, 1, void, 0, axFloat32x3, axRenderNormalType::Normal>())) \
//------
ax_ENUM_CLASS(axRenderVertexType, axUInt64)

