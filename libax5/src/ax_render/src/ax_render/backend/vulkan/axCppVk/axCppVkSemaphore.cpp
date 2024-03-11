#if AX_USE_VULKAN

#include "axCppVkSemaphore.h"

void axCppVkSemaphore::destroy() {
	if (_h) {
		vkDestroySemaphore(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkSemaphore::create(VkDevice dev) {
	destroy();
	_dev = dev;
	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	auto err = vkCreateSemaphore(_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif
