#pragma once

#if AX_USE_VULKAN

#include "../../shader/axMaterialPass.h"
#include "axVkShaderPass.h"

class axVkMaterialPass : public axMaterialPass {
	AX_RTTI_CLASS(axVkMaterialPass, axMaterialPass);
public:
	using Util = axVkUtil;

	void bind(axVkCommandDispatcher& dispatcher, DrawCall& drawCall);
	void bind(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall);

	axVkShaderPass*	shaderPass() { return ax_type_cast_debug<axVkShaderPass>(Base::shaderPass()); }

	void _updateDescriptorSet(VkDescriptorSet destSet, axVkCommandDispatcher& dispatcher);
	void _bindParams(axVkCommandDispatcher& dispatcher);

	axCppVkDescriptorPool		_descPool;
	axArray<VkDescriptorSet, 4>	_descSets;
	axInt64 _lastUsedRenderSeqId = 0;
	axInt   _descSetIndex = 0;
};

#endif
