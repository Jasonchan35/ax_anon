#pragma once

class axSpirvInfoGenerator : public axNonCopyable {
public:
	using DataType = axRenderDataType;

	axSpirvInfoGenerator();

	void generate(axStrView outFilename, axStrView filename);
	void _genEntryPoint();
	void _genVertexInputs();
	void _genUniformBuffer();
	void _genTextures();
	void _genSamplers();
	void _genStorageBuffers();

private:
	axJsonValue			_spirvReflect;
	axShaderStageInfo	_outInfo;

	DataType	toDataType(axStrView name);

	axDict<axString, DataType>	_dataTypeDict;
};
