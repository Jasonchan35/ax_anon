#include "axShaderInfo.h"
#include "../vertex/axRenderVertex.h"

const axStrLiteral axShaderStageProfile::DX11_VS		= "vs_5_0";
const axStrLiteral axShaderStageProfile::DX11_PS		= "ps_5_0";
const axStrLiteral axShaderStageProfile::DX11_CS		= "cs_5_0";

const axStrLiteral axShaderStageProfile::DX12_VS		= "vs_6_0";
const axStrLiteral axShaderStageProfile::DX12_PS		= "ps_6_0";
const axStrLiteral axShaderStageProfile::DX12_CS		= "cs_6_0";
const axStrLiteral axShaderStageProfile::DX12_Lib		= "lib_6_3";

const axStrLiteral axShaderStageProfile::GLSL_VS		= "vs_330";
const axStrLiteral axShaderStageProfile::GLSL_PS		= "ps_330";
const axStrLiteral axShaderStageProfile::GLSL_CS		= "cs_430";

const axStrLiteral axShaderStageProfile::GLSL_ES_VS		= "vs_310";
const axStrLiteral axShaderStageProfile::GLSL_ES_PS		= "ps_310";
const axStrLiteral axShaderStageProfile::GLSL_ES_CS		= "cs_310";

const axStrLiteral axShaderStageProfile::MSL_VS_Mac		= "vs_mac";
const axStrLiteral axShaderStageProfile::MSL_PS_Mac		= "ps_mac";
const axStrLiteral axShaderStageProfile::MSL_CS_Mac		= "cs_mac";
const axStrLiteral axShaderStageProfile::MSL_Lib_Mac	= "lib_mac";

const axStrLiteral axShaderStageProfile::MSL_VS_iOS		= "vs_ios";
const axStrLiteral axShaderStageProfile::MSL_PS_iOS		= "ps_ios";
const axStrLiteral axShaderStageProfile::MSL_CS_iOS		= "cs_ios";
const axStrLiteral axShaderStageProfile::MSL_Lib_iOS	= "lib_ios";

const axStrLiteral axShaderStageProfile::SPIRV_VS		= "vs_1_1";
const axStrLiteral axShaderStageProfile::SPIRV_PS		= "ps_1_1";
const axStrLiteral axShaderStageProfile::SPIRV_CS		= "cs_1_1";

const axStrLiteral axShaderStageProfile::ISPC_VS		= "vs";
const axStrLiteral axShaderStageProfile::ISPC_PS		= "ps";
const axStrLiteral axShaderStageProfile::ISPC_CS		= "cs";

axRenderVertexAttrId axShaderVertexAttrIdHelper::fromVarName(axStrView name) {
	auto* attr = s_instance()->_nameToAttrIdDict.find(name);
	if (!attr) return axRenderVertexAttrId::Unknown;
	return *attr;
}

axShaderVertexAttrIdHelper* axShaderVertexAttrIdHelper::s_instance() {
	static axGlobalSingleton<axShaderVertexAttrIdHelper> s;
	return s;
}

axShaderVertexAttrIdHelper::axShaderVertexAttrIdHelper() {
	_nameToAttrIdDict.add("i_pos",		AttrId::POSITION0);
	_nameToAttrIdDict.add("i_normal",	AttrId::NORMAL0);
	_nameToAttrIdDict.add("i_tangent",	AttrId::TANGENT0);
	_nameToAttrIdDict.add("i_uv",		AttrId::TEXCOORD0);
	for (axInt i = 0; i < axRenderVertexAttrId_kUvCount; i++) {
		_nameToAttrIdDict.add(axTempString::s_format("i_uv{?}", i),	AttrId::TEXCOORD0 + static_cast<axEnum<AttrId>::IntType>(i));
	}

	_nameToAttrIdDict.add("i_custom",	AttrId::CUSTOM0);
	for (axInt i = 0; i < axRenderVertexAttrId_kCustomCount; i++) {
		_nameToAttrIdDict.add(axTempString::s_format("i_custom{?}", i),	AttrId::CUSTOM0 + static_cast<axEnum<AttrId>::IntType>(i));
	}

	_nameToAttrIdDict.add("i_color",	AttrId::COLOR0);
	for (axInt i = 0; i < axRenderVertexAttrId_kColorCount; i++) {
		_nameToAttrIdDict.add(axTempString::s_format("i_color{?}", i),	AttrId::COLOR0 + static_cast<axEnum<AttrId>::IntType>(i));
	}
}

axShaderStageInfoSet::axShaderStageInfoSet() {
	vsInfo.stageFlags = axShaderStageFlags::Vertex;
	psInfo.stageFlags = axShaderStageFlags::Pixel;
	csInfo.stageFlags = axShaderStageFlags::Compute;
}
