#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkSurfaceKHR : public axCppVkBase {
public:
	const VkSurfaceKHR& handle() { return _h; }
	operator const VkSurfaceKHR&() { return _h; }

	~axCppVkSurfaceKHR() { destroy(); }
	void destroy();

	axCppVkPhysicalDevice& physicalDevice() { return *_phyDev; }

#if AX_OS_WINDOWS
	void create_Win32(axCppVkPhysicalDevice& dev, HINSTANCE hInstance, HWND hwnd);
#endif

#if AX_USE_X11
	void create_x11(axCppVkPhysicalDevice& dev, ax_X11_Display* dpy, ax_X11_Window win);
#endif

#if AX_OS_OSX
	void create_macOS(axCppVkPhysicalDevice& dev, NSView* view);
//	void create_metal(axCppVkPhysicalDevice& dev, const CAMetalLayer* layer);
#endif

	VkSurfaceCapabilitiesKHR	getCapabilities();

	axSpan<VkPresentModeKHR>	supportedPresentModes() { return _supportedPresentModes; }
	axSpan<VkSurfaceFormatKHR>	supportedFormats()		{ return _supportedFormats; }

	bool checkQueueFamilyIndexSupportPresent	(uint32_t queueFamilyIndex);
	bool findQueueFamilyIndexSupportPresent		(uint32_t& outIndex);

	bool isSupportPresentMode(VkPresentModeKHR presentMode);

	bool isSupportFormat		(const VkSurfaceFormatKHR& f) { return isSupportFormat(f.format, f.colorSpace); }
	bool isSupportFormat		(VkFormat format, VkColorSpaceKHR colorSpace);

private:
	void _onCreate();

	VkSurfaceKHR	_h = nullptr;
	axCppVkPhysicalDevice* _phyDev = nullptr;
	axArray<VkSurfaceFormatKHR, 32> _supportedFormats;
	axArray<VkPresentModeKHR,	8 > _supportedPresentModes;
};

#endif
