#pragma once

#if AX_USE_VULKAN

#include "axCppVkSurfaceKHR.h"

class axCppVkSwapchainKHR : public axCppVkBase {
public:
	const VkSwapchainKHR& handle() { return _h; }
	operator const VkSwapchainKHR&() { return _h; }

	axCppVkSwapchainKHR() = default;
	axCppVkSwapchainKHR(axCppVkSwapchainKHR&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = nullptr;
	}

	~axCppVkSwapchainKHR() { destroy(); }

	void destroy();
	void create(axCppVkDevice& dev,
				axCppVkSurfaceKHR& surface,
				axCppVkSwapchainKHR* oldSwapchain,
				const VkSurfaceFormatKHR& format,
				uint32_t imageCount,
				VkPresentModeKHR presentMode,
				const axVkQueueFamilyIndices& queueFamilyIndices);

	void getImages(axIArray<VkImage>& outImages);


private:
	VkSwapchainKHR _h = nullptr;
	axCppVkDevice* _dev = nullptr;
};

#endif