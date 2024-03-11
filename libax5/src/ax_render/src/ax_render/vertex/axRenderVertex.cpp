#include "axRenderVertex.h"

static axInt axRenderVertexType_GetAttrCountHelper(axRenderVertexType type, axRenderVertexAttrId start, axRenderVertexAttrId end) {
	auto startBit = ax_enum_int(start);
	auto n = ax_enum_int(end - start);
	AX_ASSERT(ax_is_pow2(n));

	auto mask = (axEnum<axRenderVertexType>::kOne << n) - 1;
	auto o = (type >> startBit) & mask;
	return static_cast<axInt>(ax_enum_int(o));
}

axInt axRenderVertexType_GetUvCount(axRenderVertexType type) {
	return axRenderVertexType_GetAttrCountHelper(type, axRenderVertexAttrId::TEXCOORD0, axRenderVertexAttrId::TEXCOORD_END);
}

axInt axRenderVertexType_GetColorCount(axRenderVertexType type) {
	return axRenderVertexType_GetAttrCountHelper(type, axRenderVertexAttrId::COLOR0, axRenderVertexAttrId::COLOR_END);
}

axInt axRenderVertexType_GetCustomCount(axRenderVertexType type) {
	return axRenderVertexType_GetAttrCountHelper(type, axRenderVertexAttrId::CUSTOM0, axRenderVertexAttrId::CUSTOM_END);
}
