#if AX_USE_VULKAN

#include "axCppVkDescriptorSetLayout.h"

axCppVkDescriptorSetLayout::axCppVkDescriptorSetLayout(axCppVkDescriptorSetLayout&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = nullptr;
	r._dev = nullptr;
}

void axCppVkDescriptorSetLayout::destroy() {
	if (_h) {
		vkDestroyDescriptorSetLayout(*_dev, _h, Util::allocCallbacks());
		_h = nullptr;
		_dev = nullptr;
	}
}

void axCppVkDescriptorSetLayout::create(axCppVkDevice& dev, axSpan<VkDescriptorSetLayoutBinding> bindings) {
	destroy();
	_dev = &dev;

	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext			= nullptr;
	info.flags			= 0;
	info.bindingCount	= Util::castUInt32(bindings.size());
	info.pBindings		= bindings.data();

	auto err = vkCreateDescriptorSetLayout(*_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif