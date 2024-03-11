#if AX_USE_VULKAN

#include "axCppVkFence.h"

void axCppVkFence::destroy() {
	if (_h != VK_NULL_HANDLE) {
		vkDestroyFence(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkFence::create(VkDevice dev, bool signaled) {
	destroy();
	_dev = dev;

	VkFenceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	auto err = vkCreateFence(_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

bool axCppVkFence::wait(uint64_t nanoseconds) {
	auto err = vkWaitForFences(_dev, 1, &_h, true, nanoseconds);
	if (err == VK_TIMEOUT)
		return false;
	Util::throwIfError(err);

	//auto st = vkGetFenceStatus(_dev, _h);
	//if (st == VK_ERROR_DEVICE_LOST) {
	//	throw axError_Undefined(AX_LOC);
	//}

	return true;
}

void axCppVkFence::reset() {
	auto err = vkResetFences(_dev, 1, &_h);
	Util::throwIfError(err);
}

#endif