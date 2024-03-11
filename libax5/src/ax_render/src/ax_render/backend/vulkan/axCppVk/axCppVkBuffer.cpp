#if AX_USE_VULKAN

#include "axCppVkBuffer.h"

void axCppVkBuffer::destroy() {
	if (_h) {
		vkDestroyBuffer(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkBuffer::create(axCppVkDevice& dev, VkDeviceSize dataSize, VkBufferUsageFlags usage) {
	if (!_h 
		|| _dev != &dev 
		|| dataSize != _dataSize 
		|| usage != _usage) 
	{
		destroy();
		_dev = &dev;

		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.size					= dataSize;
		info.usage					= usage;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;

		auto err = vkCreateBuffer(*_dev, &info, Util::allocCallbacks(), &_h);
		Util::throwIfError(err);

		_dataSize = dataSize;
	}
}

VkMemoryRequirements axCppVkBuffer::getMemoryRequirements() {
	VkMemoryRequirements o;
	vkGetBufferMemoryRequirements(*_dev, _h, &o);
	return o;
}

#endif