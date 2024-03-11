#if AX_USE_VULKAN

#include "axCppVkImageView.h"

void axCppVkImageView::destroy() {
	if (_h) {
		vkDestroyImageView(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkImageView::create(axCppVkImage& image) {
	create(image.device(), image, image.format());
}

void axCppVkImageView::create(VkDevice dev, VkImage image, VkFormat format) {
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.image = image;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	{
		auto& mask = info.subresourceRange.aspectMask;
		if (Util::formatHasDepth	(format)) mask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (Util::formatHasStencil(format)) mask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		if (!mask) mask |= VK_IMAGE_ASPECT_COLOR_BIT;
	}
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	create(dev, image, info);
}

void axCppVkImageView::create(VkDevice dev, VkImage image, VkImageViewCreateInfo& info) {
	destroy();
	_dev = dev;

	auto err = vkCreateImageView(_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif
