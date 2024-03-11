#pragma once

#if AX_USE_VULKAN

#include "../../shader/axShaderPass.h"
#include "axVkRenderer.h"

class axVkShaderPass : public axShaderPass {
	AX_RTTI_CLASS(axVkShaderPass, axShaderPass)
public:
	using Util = axVkUtil;

	virtual void onInit	(axInt passIndex, const Info& info) override;
	virtual bool onValidate() final;

	void bind(axVkCommandDispatcher& dispatcher, DrawCall& drawCall);
	void bind(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall);

	virtual void onInitStageInfos(StageInfoSet& infoSet) override;

	VkDescriptorSetLayout	psoDescSetLayout() { return _psoDescSetLayout; }
	VkPipelineLayout		psoLayout() { return _psoLayout; }

private:
	class VertexInputLayoutDesc {
	public:
		VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc);

		VkPipelineVertexInputStateCreateInfo	vertexInputState = {};

		axArray<VkVertexInputBindingDescription,	1>		bindingDesc;
		axArray<VkVertexInputAttributeDescription,	64>		attrDesc;
	};

//---- Pipeline State Object ----
	class PsoKey {
	public:
		VertexDesc vertexDesc = nullptr;
		PrimitiveType primitiveType = PrimitiveType::Unknown;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		bool debugWireframe = false;

		bool operator==(const PsoKey& r) const {
			return vertexDesc		== r.vertexDesc
				&& primitiveType	== r.primitiveType
				&& renderPass		== r.renderPass
				&& debugWireframe	== r.debugWireframe;
		}
	};

	class Pso {
	public:
		axCppVkPipelineCache	_cache;
		axCppVkPipeline			_pipeline;
	};

	void _createGraphicsPipelineState(Pso& pso, const PsoKey& key);
	axArrayMap<PsoKey, Pso, 4> _graphicsPipelineStateMap;

	void _createComputePipelineState(Pso& pso);
	Pso _computePipeline;

	axCppVkDescriptorSetLayout	_psoDescSetLayout;
	axCppVkPipelineLayout		_psoLayout;

	axCppVkShaderModule	_vsModule;
	axCppVkShaderModule	_psModule;
	axCppVkShaderModule	_csModule;
};

#endif //AX_USE_VULKAN
