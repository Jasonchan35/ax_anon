#include "axSpirvInfoGenerator.h"

axSpirvInfoGenerator::axSpirvInfoGenerator() {
	_dataTypeDict.add("int",	DataType::Int32);
	_dataTypeDict.add("ivec2",	DataType::Int32x2);
	_dataTypeDict.add("ivec3",	DataType::Int32x3);
	_dataTypeDict.add("ivec4",	DataType::Int32x4);

	_dataTypeDict.add("uint",	DataType::UInt32);
	_dataTypeDict.add("uvec2",	DataType::UInt32x2);
	_dataTypeDict.add("uvec3",	DataType::UInt32x3);
	_dataTypeDict.add("uvec4",	DataType::UInt32x4);

	_dataTypeDict.add("float",	DataType::Float32);
	_dataTypeDict.add("vec2",	DataType::Float32x2);
	_dataTypeDict.add("vec3",	DataType::Float32x3);
	_dataTypeDict.add("vec4",	DataType::Float32x4);

	_dataTypeDict.add("double",	DataType::Float64);
	_dataTypeDict.add("dvec2",	DataType::Float64x2);
	_dataTypeDict.add("dvec3",	DataType::Float64x3);
	_dataTypeDict.add("dvec4",	DataType::Float64x4);

	_dataTypeDict.add("mat4",	DataType::Float32_4x4);
}

void axSpirvInfoGenerator::generate(axStrView outFilename, axStrView filename) {
	_spirvReflect.initFromFile(filename);
	_genEntryPoint();
	_genVertexInputs();
	_genUniformBuffer();
	_genTextures();
	_genSamplers();
	_genStorageBuffers();

	axString8 outJson;
	axJsonUtil::writeFile(outFilename, _outInfo, false);
}

void axSpirvInfoGenerator::_genEntryPoint() {
	auto* entryPoints = _spirvReflect.tryGetMemberArray("entryPoints");
	if (!entryPoints) return;

	for (auto& e : *entryPoints) {
		e.tryGetMemberValue("workgroup_size", _outInfo.csWorkgroupSize);
	}
}

void axSpirvInfoGenerator::_genVertexInputs() {
	auto* inputs = _spirvReflect.tryGetMemberArray("inputs");
	if (!inputs) {
		//throw axError_Undefined(AX_LOC);
		return;
	}

	for (auto& obj : *inputs) {
		auto& dst = _outInfo.inputs.emplaceBack();

		if (!obj.tryGetMemberValue("name", dst.name)) {
			throw axError_Undefined(AX_LOC);
		}

		axString type;
		if (!obj.tryGetMemberValue("type", type)) {
			throw axError_Undefined(AX_LOC);
		}

		dst.attrId   = axShaderVertexAttrIdHelper::fromVarName(dst.name);
		dst.dataType = toDataType(type);
	}
}

void axSpirvInfoGenerator::_genUniformBuffer() {
	auto* ubos = _spirvReflect.tryGetMemberArray("ubos");
	if (!ubos) {
		//throw axError_Undefined(AX_LOC);
		return;
	}

	auto* typeList = _spirvReflect.tryGetMemberObject("types");

	for (auto& obj : *ubos) {
		auto& outCB = _outInfo.uniformBuffers.emplaceBack();

		if (!obj.tryGetMemberValue("name", outCB.name)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!obj.tryGetMemberValue("binding", outCB.bindPoint)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!obj.tryGetMemberValue("block_size", outCB.dataSize)) {
			throw axError_Undefined(AX_LOC);
		}

		axString typeName;
		if (!obj.tryGetMemberValue("type", typeName)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!typeList) {
			throw axError_Undefined(AX_LOC);
		}

		auto* type = typeList->tryGetMemberObject(typeName);
		if (!type) {
			throw axError_Undefined(AX_LOC);
		}

		auto* typeMembers = type->tryGetMemberArray("members");
		if (!typeMembers) {
			throw axError_Undefined(AX_LOC);
		}

		outCB.variables.reserve(typeMembers->size());
		for (auto& m : *typeMembers) {
			auto& outVar = outCB.variables.emplaceBack();
			if (!m.tryGetMemberValue("name", outVar.name)) {
				throw axError_Undefined(AX_LOC);
			}
			if (!m.tryGetMemberValue("offset", outVar.offset)) {
				throw axError_Undefined(AX_LOC);
			}
			if (!m.tryGetMemberValue("row_major", outVar.rowMajor)) {
				// optional
				// throw axError_Undefined(AX_LOC);
			}
			axString varType;
			if (!m.tryGetMemberValue("type", varType)) {
				throw axError_Undefined(AX_LOC);
			}
			outVar.dataType = toDataType(varType);
		}
	}
}

void axSpirvInfoGenerator::_genTextures() {
	auto* textures = _spirvReflect.tryGetMemberArray("separate_images");
	if (!textures) {
		//throw axError_Undefined(AX_LOC);
		return;
	}

	for (auto& tex : *textures) {
		auto& outTex = _outInfo.textures.emplaceBack();
		if (!tex.tryGetMemberValue("name", outTex.name)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!tex.tryGetMemberValue("binding", outTex.bindPoint)) {
			throw axError_Undefined(AX_LOC);
		}
		axString typeName;
		if (!tex.tryGetMemberValue("type", typeName)) {
			throw axError_Undefined(AX_LOC);
		}
		
		if (typeName == "texture1D") {
			outTex.dataType = DataType::Texture1D;
		} else if (typeName == "texture2D") {
			outTex.dataType = DataType::Texture2D;
		} else if (typeName == "texture3D") {
			outTex.dataType = DataType::Texture3D;
		} else {
			AX_LOG("Unknown type {?}", typeName);
			throw axError_Undefined(AX_LOC);
		}
	}
}

void axSpirvInfoGenerator::_genSamplers() {
	auto* samplers = _spirvReflect.tryGetMemberArray("separate_samplers");
	if (!samplers) {
		//throw axError_Undefined(AX_LOC);
		return;
	}

	for (auto& s : *samplers) {
		auto& outSampler = _outInfo.samplers.emplaceBack();

		if (!s.tryGetMemberValue("name", outSampler.name)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!s.tryGetMemberValue("binding", outSampler.bindPoint)) {
			throw axError_Undefined(AX_LOC);
		}

		outSampler.dataType = DataType::SamplerState;
	}
}

void axSpirvInfoGenerator::_genStorageBuffers() {
	auto* arr = _spirvReflect.tryGetMemberArray("ssbos");
	if (!arr) {
		//throw axError_Undefined(AX_LOC);
		return;
	}

	for (auto& s : *arr) {
		auto& outBuf = _outInfo.storageBuffers.emplaceBack();

		if (!s.tryGetMemberValue("name", outBuf.name)) {
			throw axError_Undefined(AX_LOC);
		}
		if (!s.tryGetMemberValue("binding", outBuf.bindPoint)) {
			throw axError_Undefined(AX_LOC);
		}

		if (outBuf.bindPoint == 0) {
			AX_LOG("register(u0) is reversed for Global Uniform Buffer");
			throw axError_Undefined(AX_LOC);
		}
	}
}

axRenderDataType axSpirvInfoGenerator::toDataType(axStrView name) {
	auto* s = _dataTypeDict.find(name);
	if (!s) {
		AX_LOG("Unknown type [{?}]", name);
		throw axError_Undefined(AX_LOC);
	}
	return *s;
}
