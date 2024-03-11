#if AX_USE_VULKAN

#include "axCppVkPipelineLayout.h"

axCppVkPipelineLayout::axCppVkPipelineLayout(axCppVkPipelineLayout&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void axCppVkPipelineLayout::destroy() {
	if (_h) {
		vkDestroyPipelineLayout(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkPipelineLayout::create(axCppVkDevice& dev, axSpan<VkDescriptorSetLayout> descriptorSets) {
	destroy();
	_dev = &dev;

	VkPipelineLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	info.setLayoutCount = Util::castUInt32(descriptorSets.size());
	info.pSetLayouts = descriptorSets.data();

	auto err = vkCreatePipelineLayout(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif