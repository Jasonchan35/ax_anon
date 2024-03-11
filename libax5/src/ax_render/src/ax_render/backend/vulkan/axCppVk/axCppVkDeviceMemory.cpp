#if AX_USE_VULKAN

#include "axCppVkDeviceMemory.h"

void axCppVkDeviceMemory::destroy() {
	if (_h) {
		vkFreeMemory(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkDeviceMemory::_create(axCppVkDevice& dev, VkDeviceSize dataSize, VkFlags memoryTypeBits, VkMemoryPropertyFlags requireMask) {
	uint32_t memTypeIndex = UINT32_MAX;
	if (!dev.physicalDevice()->findMemoryTypeIndex(memTypeIndex, memoryTypeBits, requireMask))
		throw axError_Undefined(AX_LOC);

	_create(dev, dataSize, memTypeIndex);
}

void axCppVkDeviceMemory::copyData(const void* data, VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags) {
	if (size <= 0) return;
	auto* dst = map(size, offset, flags);
	ax_memcpy(dst, data, static_cast<axInt>(size));
	unmap();
}

void* axCppVkDeviceMemory::map(VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags) {
	void* outPtr = nullptr;
	auto err = vkMapMemory(*_dev, _h, offset, size, flags, &outPtr);
	Util::throwIfError(err);
	return outPtr;
}

void axCppVkDeviceMemory::createForImage(axCppVkImage& img, VkMemoryPropertyFlags requireMask) {
	auto req = img.getMemoryRequirements();
	auto& dev = img.device();
	_create(dev, req.size, req.memoryTypeBits, requireMask);

	auto err = vkBindImageMemory(dev, img, _h, 0);
	Util::throwIfError(err);
}

void axCppVkDeviceMemory::createForBuffer(axCppVkBuffer& buf, VkMemoryPropertyFlags requireMask) {
	auto req = buf.getMemoryRequirements();
	auto& dev = buf.device();
	_create(dev, req.size, req.memoryTypeBits, requireMask);

	auto err = vkBindBufferMemory(dev, buf, _h, 0);
	Util::throwIfError(err);
}

inline void axCppVkDeviceMemory::unmap() {
	vkUnmapMemory(*_dev, _h);
}

void axCppVkDeviceMemory::_create(axCppVkDevice& dev, VkDeviceSize dataSize, uint32_t memTypeIndex) {
	destroy();
	_dev = &dev;

	VkMemoryAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.allocationSize = dataSize;
	info.memoryTypeIndex = memTypeIndex;
	auto err = vkAllocateMemory(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif
