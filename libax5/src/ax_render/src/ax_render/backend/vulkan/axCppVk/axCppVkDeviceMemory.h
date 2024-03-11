#pragma once

#if AX_USE_VULKAN

#include "axCppVkImage.h"
#include "axCppVkBuffer.h"

class axCppVkDeviceMemory : public axCppVkBase {
public:
	const VkDeviceMemory& handle() { return _h; }
	operator const VkDeviceMemory&() { return _h; }

	axCppVkDeviceMemory() = default;
	axCppVkDeviceMemory(axCppVkDeviceMemory&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = nullptr;
	}

	~axCppVkDeviceMemory() { destroy(); }

	void destroy();
	void createForImage	(axCppVkImage&  img, VkMemoryPropertyFlags requireMask);
	void createForBuffer(axCppVkBuffer& buf, VkMemoryPropertyFlags requireMask);


	void copyData(const void* data, VkDeviceSize size, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);

	void* map(VkDeviceSize size, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
	void  unmap();

private:
	void _create(axCppVkDevice& dev, VkDeviceSize dataSize, VkFlags memoryTypeBits, VkMemoryPropertyFlags requireMask);
	void _create(axCppVkDevice& dev, VkDeviceSize dataSize, uint32_t memTypeIndex);

	VkDeviceMemory _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif