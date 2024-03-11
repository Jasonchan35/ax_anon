#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkCommandPool : public axCppVkBase {
public:
	const VkCommandPool& handle() { return _h; }
	operator const VkCommandPool&() { return _h; }

	axCppVkCommandPool() = default;

	axCppVkCommandPool(axCppVkCommandPool && r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = nullptr;
	}

	~axCppVkCommandPool() { destroy(); }

	void destroy();

	axCppVkDevice& device() { return *_dev; }
	void create(axCppVkDevice& dev, uint32_t queueFamilyIndex);

private:
	VkCommandPool _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};



#endif