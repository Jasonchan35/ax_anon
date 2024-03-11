#if AX_USE_VULKAN

#include "axCppVkSwapchainKHR.h"

void axCppVkSwapchainKHR::destroy() {
	if (_h) {
		vkDestroySwapchainKHR(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkSwapchainKHR::create(	axCppVkDevice& dev,
									axCppVkSurfaceKHR& surface,
									axCppVkSwapchainKHR* oldSwapchain,
									const VkSurfaceFormatKHR& format,
									uint32_t imageCount,
									VkPresentModeKHR presentMode,
									const axVkQueueFamilyIndices& queueFamilyIndices)
{
	destroy();
	_dev = &dev;

	if (!surface) throw axError_Undefined(AX_LOC);
	if (!surface.isSupportFormat(format))	throw axError_Undefined(AX_LOC);

	auto cap = surface.getCapabilities();
	if (imageCount < cap.minImageCount) throw axError_Undefined(AX_LOC);
	if (imageCount > cap.maxImageCount) throw axError_Undefined(AX_LOC);

	axArray<uint32_t, 4> qfi;

	qfi.append(queueFamilyIndices.graphIndex());
	if (queueFamilyIndices.presentQueueIsSeparated()) {
		qfi.append(queueFamilyIndices.presentIndex());
	}

	VkExtent2D imageExtent = cap.currentExtent;

	VkSwapchainCreateInfoKHR info = {};
	info.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.pNext					= nullptr;
	info.flags					= 0;
	info.surface				= surface;
	info.minImageCount			= imageCount;
	info.imageFormat			= format.format;
	info.imageColorSpace		= format.colorSpace;
	info.imageExtent			= imageExtent;
	info.imageArrayLayers		= 1;
	info.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount	= Util::castUInt32(qfi.size());
	info.pQueueFamilyIndices	= qfi.data();
	info.preTransform			= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	info.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode			= presentMode;
	info.clipped				= VK_TRUE;
	info.oldSwapchain			= oldSwapchain ? oldSwapchain->handle() : nullptr;

	auto err = vkCreateSwapchainKHR(*_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

void axCppVkSwapchainKHR::getImages(axIArray<VkImage>& outImages) {
	outImages.clear();

	VkResult err;

	uint32_t count = 0;
	err = vkGetSwapchainImagesKHR(*_dev, _h, &count, nullptr);
	Util::throwIfError(err);

	outImages.resize(count);
	err = vkGetSwapchainImagesKHR(*_dev, _h, &count, outImages.data());
	Util::throwIfError(err);
}

#endif
