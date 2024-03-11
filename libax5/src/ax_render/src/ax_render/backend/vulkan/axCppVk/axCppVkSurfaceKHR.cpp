#if AX_USE_VULKAN

#include "axCppVkSurfaceKHR.h"

void axCppVkSurfaceKHR::destroy() {
	if (_h) {
		vkDestroySurfaceKHR(*_phyDev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_phyDev = nullptr;
	}
}

#if AX_OS_WINDOWS
void axCppVkSurfaceKHR::create_Win32(axCppVkPhysicalDevice& phyDev, HINSTANCE hInstance, HWND hwnd) {
	destroy();
	_phyDev = &phyDev;

	VkWin32SurfaceCreateInfoKHR ci = {};
	ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.hinstance = hInstance;
	ci.hwnd = hwnd;

	auto err = vkCreateWin32SurfaceKHR(*_phyDev, &ci, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);

	_onCreate();
}
#endif

#if AX_USE_X11
void axCppVkSurfaceKHR::create_x11(axCppVkPhysicalDevice& phyDev, ax_X11_Display* dpy, ax_X11_Window win) {
	destroy();

	if (!dpy) throw axError_Undefined(AX_LOC);
	if (!win) win = DefaultRootWindow(dpy);

	_phyDev = &phyDev;
	VkXlibSurfaceCreateInfoKHR ci = {};
	ci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.dpy = dpy;
	ci.window = win;

	auto err = vkCreateXlibSurfaceKHR(*_phyDev, &ci, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);

	_onCreate();
}
#endif

#if AX_OS_OSX
void axCppVkSurfaceKHR::create_macOS(axCppVkPhysicalDevice& phyDev, NSView* view) {
	destroy();
	if (!view) {
		throw axError_Undefined(AX_LOC);
	}

	if (!([view.layer isKindOfClass: [CAMetalLayer class]])) {
		throw axError_Undefined(AX_LOC);
	}

	_phyDev = &phyDev;
	VkMacOSSurfaceCreateInfoMVK ci = {};
	ci.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
	ci.pView = (__bridge void*)view;
	auto err = vkCreateMacOSSurfaceMVK(*_phyDev, &ci, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);

	_onCreate();
}

//void axCppVkSurfaceKHR::create_metal(axCppVkPhysicalDevice& phyDev, const CAMetalLayer* layer) {
//	destroy();
//
//	_phyDev = &phyDev;
//	VkMetalSurfaceCreateInfoEXT ci = {};
//	ci.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
//	ci.pLayer = layer;
//	auto err = vkCreateMetalSurfaceEXT(*_phyDev, &ci, Util::allocCallbacks(), &_h);
//	Util::throwIfError(err);
//
//	_onCreate();
//}

#endif


VkSurfaceCapabilitiesKHR axCppVkSurfaceKHR::getCapabilities() {
	VkSurfaceCapabilitiesKHR o = {};
	auto err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*_phyDev, _h, &o);
	Util::throwIfError(err);
	return o;
}

bool axCppVkSurfaceKHR::checkQueueFamilyIndexSupportPresent(uint32_t queueFamilyIndex) {
	// physical device supports presentation to a given surface
	VkBool32 b = false;
	auto err = vkGetPhysicalDeviceSurfaceSupportKHR(*_phyDev, queueFamilyIndex, _h, &b);
	Util::throwIfError(err);
	return b != 0;
}

bool axCppVkSurfaceKHR::findQueueFamilyIndexSupportPresent(uint32_t& outIndex) {
	uint32_t n = Util::castUInt32(_phyDev->queueFamilyProps().size());

	for (uint32_t i = 0; i < n; i++) {
		if (checkQueueFamilyIndexSupportPresent(i)) {
			outIndex = i;
			return true;
		}
	}
	return false;
}

bool axCppVkSurfaceKHR::isSupportPresentMode(VkPresentModeKHR presentMode) {
	for (auto& e : _supportedPresentModes) {
		if (e == presentMode) {
			return true;
		}
	}
	return false;
}

bool axCppVkSurfaceKHR::isSupportFormat(VkFormat format, VkColorSpaceKHR colorSpace) {
	for (auto& e : _supportedFormats) {
		if (e.format == format && e.colorSpace == colorSpace) {
			return true;
		}
	}
	return false;
}

void axCppVkSurfaceKHR::_onCreate() {
	VkResult err;
	{
		uint32_t count;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*_phyDev, _h, &count, nullptr);
		Util::throwIfError(err);

		_supportedFormats.resize(count);
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*_phyDev, _h, &count, _supportedFormats.data());
		Util::throwIfError(err);
	}
	{
		uint32_t count;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*_phyDev, _h, &count, nullptr);
		Util::throwIfError(err);

		_supportedPresentModes.resize(count);
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*_phyDev, _h, &count, _supportedPresentModes.data());
		Util::throwIfError(err);
	}
}

#endif
