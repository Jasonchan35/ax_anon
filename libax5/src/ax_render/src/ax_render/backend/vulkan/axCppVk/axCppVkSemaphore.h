#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkSemaphore : public axCppVkBase {
public:
	const VkSemaphore& handle() { return _h; }
	operator const VkSemaphore&() { return _h; }

	axCppVkSemaphore() = default;
	axCppVkSemaphore(axCppVkSemaphore&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
	}
	~axCppVkSemaphore() { destroy(); }

	void destroy();
	void create(VkDevice dev);

private:
	VkSemaphore _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif