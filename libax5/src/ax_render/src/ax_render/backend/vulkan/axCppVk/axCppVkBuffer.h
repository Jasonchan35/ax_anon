#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkBuffer : public axCppVkBase {
public:
	const VkBuffer& handle() { return _h; }
	operator const VkBuffer&() { return _h; }
	~axCppVkBuffer() { destroy(); }

	axCppVkBuffer() = default;

	axCppVkBuffer(axCppVkBuffer&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = nullptr;
	}

	void destroy();
	void create(axCppVkDevice& dev, VkDeviceSize dataSize, VkBufferUsageFlags usage);

	axCppVkDevice& device() { return *_dev; }
	VkMemoryRequirements getMemoryRequirements();

private:
	VkBuffer			_h = VK_NULL_HANDLE;
	axCppVkDevice*		_dev = nullptr;
	VkDeviceSize		_dataSize = 0;
	VkBufferUsageFlags	_usage = 0;
};

#endif