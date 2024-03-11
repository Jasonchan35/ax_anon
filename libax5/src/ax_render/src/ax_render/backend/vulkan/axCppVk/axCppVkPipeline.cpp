#if AX_USE_VULKAN

#include "axCppVkPipeline.h"

axCppVkPipeline::axCppVkPipeline(axCppVkPipeline&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void axCppVkPipeline::destroy() {
	if (_h) {
		vkDestroyPipeline(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkPipeline::create(axCppVkDevice& dev, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& info) {
	destroy();
	_dev = &dev;
	auto err = vkCreateGraphicsPipelines(dev, cache, 1, &info, Util::allocCallbacks(), &_h); 
	Util::throwIfError(err);
}

void axCppVkPipeline::create(axCppVkDevice& dev, VkPipelineCache cache, const VkComputePipelineCreateInfo& info) {
	destroy();
	_dev = &dev;
	auto err = vkCreateComputePipelines(dev, cache, 1, &info, Util::allocCallbacks(), &_h); 
	Util::throwIfError(err);
}

#endif
