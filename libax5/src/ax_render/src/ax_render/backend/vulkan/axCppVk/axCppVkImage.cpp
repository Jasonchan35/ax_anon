#if AX_USE_VULKAN

#include "axCppVkImage.h"
#include "axCppVkCommandBuffer.h"

void axCppVkImage::destroy() {
	if (_h) {
		vkDestroyImage(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkImage::createImage2D(axCppVkDevice& dev, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageLayout initialLayout) {
	VkImageCreateInfo info = {};
	info.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext			= nullptr;
	info.imageType		= VK_IMAGE_TYPE_2D;
	info.format			= format;
	info.extent.width	= width;
	info.extent.height	= height;
	info.extent.depth	= 1;
	info.mipLevels		= 1;
	info.arrayLayers	= 1;
	info.samples		= VK_SAMPLE_COUNT_1_BIT;
	info.tiling			= VK_IMAGE_TILING_OPTIMAL;
	info.usage			= usage;
	info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	info.flags			= 0;
	info.initialLayout	= initialLayout;
	_create(dev, info);
	_layout = initialLayout;
}

void axCppVkImage::_create(axCppVkDevice& dev, VkImageCreateInfo& info) {
	destroy();
	_dev = &dev;
	_format = info.format;

	auto err = vkCreateImage(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

void axCppVkImage::setLayout(axCppVkCommandBuffer& cmdBuffer, VkImageLayout newLayout, VkPipelineStageFlags newStage) {
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = _layout;
	barrier.newLayout = newLayout;
	barrier.image = _h;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.layerCount		= 1;

	switch (newLayout) {
		/* Make sure anything that was copying from this image has completed */
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;						break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
		default:												barrier.dstAccessMask = 0;																	break;
	}
	
	cmdBuffer.setCheckPoint("VkImage::setLayout");

	vkCmdPipelineBarrier(	cmdBuffer,
							_stage,
							newStage,
							0,
							0, nullptr, // memory barrier
							0, nullptr, // buffer barrier
							1, &barrier);
	_layout = newLayout;
	_stage = newStage;
}

VkMemoryRequirements axCppVkImage::getMemoryRequirements() {
	VkMemoryRequirements o = {};
	vkGetImageMemoryRequirements(*_dev, _h, &o);
	return o;
}


#endif
