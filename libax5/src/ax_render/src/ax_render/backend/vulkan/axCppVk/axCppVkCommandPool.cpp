#if AX_USE_VULKAN

#include "axCppVkCommandPool.h"

void axCppVkCommandPool::destroy() {
	if (_h) {
		vkDestroyCommandPool(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkCommandPool::create(axCppVkDevice& dev, uint32_t queueFamilyIndex) {
	destroy();
	_dev = &dev;
	VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.pNext = nullptr;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

	auto err = vkCreateCommandPool(dev, &cmdPoolCreateInfo, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif
