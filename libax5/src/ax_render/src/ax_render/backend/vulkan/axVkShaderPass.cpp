#include "../../ax_render_config.h"

#if AX_USE_VULKAN

#include "axVkShader.h"
#include "axVkShaderPass.h"
#include "axVkRenderContext.h"
#include "axVkRenderer.h"
#include "axVkCommonBase.h"
#include "axVkCommandDispatcher.h"

void axVkShaderPass::onInit(axInt passIndex, const Info& info) {
	auto shaderFilename = shader()->filename();

	axTempString vsBytecodeFilename;
	axTempString psBytecodeFilename;
	axTempString csBytecodeFilename;

	vsBytecodeFilename.format("{?}/SPIRV_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::SPIRV_VS);
	psBytecodeFilename.format("{?}/SPIRV_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::SPIRV_PS);
	csBytecodeFilename.format("{?}/SPIRV_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::SPIRV_CS);

	axTempString vsInfoFilename(vsBytecodeFilename, ".json");
	axTempString psInfoFilename(psBytecodeFilename, ".json");
	axTempString csInfoFilename(csBytecodeFilename, ".json");

	auto& device = Util::rootDevice();
	if (info.vsFunc) _vsModule.createFromFile(device, vsBytecodeFilename);
	if (info.psFunc) _psModule.createFromFile(device, psBytecodeFilename);
	if (info.csFunc) _csModule.createFromFile(device, csBytecodeFilename);

	initStageInfos(info, vsInfoFilename, psInfoFilename, csInfoFilename);
}

bool axVkShaderPass::onValidate() {
	return true;
}

void axVkShaderPass::onInitStageInfos(StageInfoSet& infoSet) {
	Base::onInitStageInfos(infoSet);
	auto& device = Util::rootDevice();

	axArray<VkDescriptorSetLayoutBinding, 32> descSetLayoutBindings;

	for (auto& cb : _uniformBufferMap) {
		auto& dst = descSetLayoutBindings.emplaceBack();
		dst.binding = Util::castUInt32(cb.bindPoint);
		dst.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		dst.descriptorCount = 1;
		dst.stageFlags = Util::getStageFlags(cb.stageFlags);
		dst.pImmutableSamplers = nullptr;
	}

	for (auto& tp : _texParams) {
		auto& p = _paramMap[tp.paramIndex];
		auto& dst = descSetLayoutBindings.emplaceBack();
		dst.binding = Util::castUInt32(p.bindPoint);
		dst.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dst.descriptorCount = 1;
		dst.stageFlags = Util::getStageFlags(p.stageFlags);
		dst.pImmutableSamplers = nullptr;
	}

	for (auto& tp : _storageBufParams) {
		auto& p = _paramMap[tp.paramIndex];
		auto& dst = descSetLayoutBindings.emplaceBack();
		dst.binding = Util::castUInt32(p.bindPoint);
		dst.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		dst.descriptorCount = 1;
		dst.stageFlags = Util::getStageFlags(p.stageFlags);
		dst.pImmutableSamplers = nullptr;
	}

	_psoDescSetLayout.create(device, descSetLayoutBindings);
	_psoLayout.create(device, axSpan_make(_psoDescSetLayout.handle()));

	if (isCompute()) {
		_createComputePipelineState(_computePipeline);
	}
}

axVkShaderPass::VertexInputLayoutDesc::VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc) {
	{
		auto& dst = bindingDesc.emplaceBack();
		dst.binding		= ax_enum_int(axVkBufferIndexUsage::VertexAttr);
		dst.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		dst.stride		= Util::castUInt32(inputVertexDesc->strideBytes);
	}

	uint32_t loc = 0;
	for (auto& reqAttr : requiredVertexAttrs) {
		auto attrId = reqAttr.attrId;
		auto& attr = inputVertexDesc->attr(attrId);

		axShaderPass::validateVertexAttr(reqAttr, attr);

		auto& dst = attrDesc.emplaceBack();

		dst.binding = ax_enum_int(axVkBufferIndexUsage::VertexAttr);
		dst.format	= Util::getVkDataType(attr.dataType);
		dst.offset  = Util::castUInt32(attr.offset);
		dst.location = loc;

		loc++;
	}

	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.vertexBindingDescriptionCount		= Util::castUInt32(bindingDesc.size());
	vertexInputState.pVertexBindingDescriptions			= bindingDesc.data();
	vertexInputState.vertexAttributeDescriptionCount	= Util::castUInt32(attrDesc.size());
	vertexInputState.pVertexAttributeDescriptions		= attrDesc.data();
}

void axVkShaderPass::bind(axVkCommandDispatcher& dispatcher, DrawCall& drawCall) {
	PsoKey key;
	key.vertexDesc = drawCall.vertexDesc;
	key.primitiveType = drawCall.primitiveType;
	key.debugWireframe = drawCall.debugWireframe || _staticRenderState.wireframe;
	key.renderPass = dispatcher.renderPass;

	auto* pso = _graphicsPipelineStateMap.find(key);
	if (!pso) {
		pso = &_graphicsPipelineStateMap.add(key);
		_createGraphicsPipelineState(*pso, key);
	}

	dispatcher.cmdBuffer->setCheckPoint("bind pipeline");
	vkCmdBindPipeline(*dispatcher.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pso->_pipeline);
}

void axVkShaderPass::bind(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	vkCmdBindPipeline(*dispatcher.cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _computePipeline._pipeline);
}

#define axVkDevice_RequireFeature(feature, state, value) \
	if (state != value && !feature) { \
		AX_ASSERT_MSG(false, "require: " #feature " for " #state " != " #value); \
		state = value; \
	} \
//-----

void axVkShaderPass::_createComputePipelineState(Pso& pso) {
	auto& device = Util::rootDevice();

	VkComputePipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.flags = 0;
	info.layout = _psoLayout;

	info.stage = {};
	info.stage.pName = _csFuncName.c_str();
	info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;;
	info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	info.stage.module = _csModule;

	pso._cache.create(device);
	pso._pipeline.create(device, pso._cache, info);
}

void axVkShaderPass::_createGraphicsPipelineState(Pso& pso, const PsoKey& key) {
	auto& device = Util::rootDevice();

	VkGraphicsPipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	info.layout = _psoLayout;
	info.renderPass = key.renderPass;

//---------
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	info.pDynamicState = &dynamicState;

	axArray<VkDynamicState, 2>	dynamicStateEnables;

	dynamicStateEnables.append(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStateEnables.append(VK_DYNAMIC_STATE_SCISSOR);

	dynamicState.dynamicStateCount	= Util::castUInt32(dynamicStateEnables.size());
	dynamicState.pDynamicStates		= dynamicStateEnables.data();

//---------
	VkPipelineInputAssemblyStateCreateInfo	inputAssemblyState	= {};
	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pInputAssemblyState = &inputAssemblyState;
	inputAssemblyState.topology = Util::getVkPrimitiveTopology(key.primitiveType);

//------
	VertexInputLayoutDesc vertexInputLayoutDesc(_vertexAttrs, key.vertexDesc);
	info.pVertexInputState = &vertexInputLayoutDesc.vertexInputState;


//-------
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info.pViewportState = &viewportState;

	VkViewport	viewport = {};
	VkRect2D	scissorRect = {};

	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissorRect;

//-------
	axArray<VkPipelineShaderStageCreateInfo, 8>		shaderStages;
	{
		auto& dst	= shaderStages.emplaceBack();
		dst.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		dst.stage	= VK_SHADER_STAGE_VERTEX_BIT;
	//	dst.flags	= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		dst.module	= _vsModule;
		dst.pName	= _vsFuncName.c_str();
	}
	{
		auto& dst	= shaderStages.emplaceBack();
		dst.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		dst.stage	= VK_SHADER_STAGE_FRAGMENT_BIT;
	//	dst.flags	= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		dst.module	= _psModule;
		dst.pName	= _psFuncName.c_str();
	}

	info.stageCount = Util::castUInt32(shaderStages.size());
	info.pStages = shaderStages.data();

//--------
	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType		= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pMultisampleState = &multisampleState;

	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	multisampleState.alphaToOneEnable = VK_FALSE;
	axVkDevice_RequireFeature(device.enabledFeatures().alphaToOne, multisampleState.alphaToOneEnable, VK_FALSE);

//--------
	VkPipelineRasterizationStateCreateInfo	rasterizationState	= {};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.pRasterizationState	= &rasterizationState;

	rasterizationState.depthClampEnable			= VK_FALSE;
	axVkDevice_RequireFeature(device.enabledFeatures().depthClamp, rasterizationState.depthClampEnable, VK_FALSE);

	rasterizationState.rasterizerDiscardEnable	= VK_FALSE;

	rasterizationState.polygonMode	= key.debugWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	axVkDevice_RequireFeature(device.enabledFeatures().fillModeNonSolid, rasterizationState.polygonMode, VK_POLYGON_MODE_FILL);

	rasterizationState.cullMode					= Util::getVkCullMode(_staticRenderState.cull); 
	rasterizationState.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthBiasEnable			= VK_FALSE;
	rasterizationState.depthBiasConstantFactor	= 0;

	rasterizationState.depthBiasClamp			= 0;
	axVkDevice_RequireFeature(device.enabledFeatures().depthBiasClamp, rasterizationState.depthBiasClamp, 0);

	rasterizationState.depthBiasSlopeFactor		= 0;

	rasterizationState.lineWidth				= 1.0f;
	axVkDevice_RequireFeature(device.enabledFeatures().wideLines, rasterizationState.lineWidth, 1.0f);

//-----
	VkPipelineColorBlendStateCreateInfo	colorBlendState	= {};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pColorBlendState		= &colorBlendState;

	axArray<VkPipelineColorBlendAttachmentState, 16> colorBlendAttachmentStates;

	{
		auto& blend = _staticRenderState.blend;
		auto& cb = colorBlendAttachmentStates.emplaceBack();
		if (blend.isEnable()) {
			cb.blendEnable			= blend.isEnable();
			cb.colorBlendOp			= Util::getVkBlendOp(blend.rgb.op);
			cb.alphaBlendOp			= Util::getVkBlendOp(blend.alpha.op);
			cb.srcColorBlendFactor	= Util::getVkBlendFactor(blend.rgb.srcFactor);
			cb.dstColorBlendFactor	= Util::getVkBlendFactor(blend.rgb.dstFactor);
			cb.srcAlphaBlendFactor	= Util::getVkBlendFactor(blend.alpha.srcFactor);
			cb.dstAlphaBlendFactor	= Util::getVkBlendFactor(blend.alpha.dstFactor);
		} else {
			cb.blendEnable = false;
		}
		cb.colorWriteMask	= VK_COLOR_COMPONENT_R_BIT
							| VK_COLOR_COMPONENT_G_BIT
							| VK_COLOR_COMPONENT_B_BIT
							| VK_COLOR_COMPONENT_A_BIT;
	}

	colorBlendState.attachmentCount = Util::castUInt32(colorBlendAttachmentStates.size());
	colorBlendState.pAttachments = colorBlendAttachmentStates.data();

//---------
	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	depthStencilState.sType	= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.pDepthStencilState = &depthStencilState;

	{
		auto& depthTest = _staticRenderState.depthTest;
		depthStencilState.depthWriteEnable	= depthTest.writeMask;
		depthStencilState.depthTestEnable	= depthTest.isEnable();
		depthStencilState.depthCompareOp	= Util::getVkDepthTestOp(depthTest.op);
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		axVkDevice_RequireFeature(device.enabledFeatures().depthBounds, depthStencilState.depthBoundsTestEnable, VK_FALSE);
	}

//---------

	pso._cache.create(device);
	pso._pipeline.create(device, pso._cache, info);
}

#endif //AX_USE_VULKAN
