#if AX_USE_VULKAN

#include "axCppVkSampler.h"

void axCppVkSampler::destroy() {
	if (_h) {
		vkDestroySampler(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkSampler::create(VkDevice dev, const VkSamplerCreateInfo& info) {
	destroy();
	_dev = dev;

	vkCreateSampler(dev, &info, Util::allocCallbacks(), &_h);
}

#endif