#pragma once

#if AX_USE_VULKAN

#include "axCppVkPhysicalDevice.h"

class axCppVkDevice : public axCppVkBase {
public:
	const VkDevice& handle() { return _h; }
	operator const VkDevice&() { return _h; }

	axCppVkPhysicalDevice* physicalDevice() { return _phyDev; }
	operator VkPhysicalDevice() { return *_phyDev; }

	VkInstance inst() { return _phyDev->inst(); }
	operator VkInstance() { return inst(); }

	~axCppVkDevice() { destroy(); }
	void destroy();

	void create(axCppVkPhysicalDevice& phyDev, const axVkQueueFamilyIndices& queueFamilyIndices);

	VkResult waitIdle() { return vkDeviceWaitIdle(_h); }

	const VkPhysicalDeviceFeatures& enabledFeatures() const { return _enabledFeatures; }

private:
	bool _addEnabledExtensions(axIArray<const char*>& arr, const char* name);

	VkDevice _h = VK_NULL_HANDLE;
	axCppVkPhysicalDevice* _phyDev = nullptr;
	VkPhysicalDeviceFeatures _enabledFeatures = {};
};

#endif
