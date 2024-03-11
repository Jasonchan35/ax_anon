#pragma once

#if AX_USE_VULKAN

#include "axCppVkInstance.h"

class axCppVkPhysicalDevice : public axCppVkBase {
public:
	const VkPhysicalDevice& handle()	{ return _h; }
	operator const VkPhysicalDevice&() { return _h; }

	void create(axInt index_, VkInstance inst, VkPhysicalDevice dev);

	bool hasExtension(const char* name);

	void logInfo() const;

	bool findQueueFamilyIndex		(uint32_t& outIndex, VkQueueFlags hasFlags) const;
	bool findComputeQueueFamilyIndex(uint32_t& outIndex) const;

	bool findMemoryTypeIndex	(uint32_t& outIndex, VkFlags typeBits, VkMemoryPropertyFlags requirements_mask) const;

	VkInstance inst() { return _inst; }
	operator VkInstance() { return inst(); }

	const VkPhysicalDeviceFeatures& features() const { return _features; }

	axStrView deviceName() const { return axStrView_c_str(_props.deviceName); }

	axSpan<VkQueueFamilyProperties>	queueFamilyProps() const	{ return _queueFamilyProps; }
	axSpan<VkExtensionProperties>	extensionProps	() const	{ return _extensionProps; }

	const VkPhysicalDeviceProperties&		props	() const	{ return _props; }
	const VkPhysicalDeviceMemoryProperties& memProps() const	{ return _memProps; }

	axInt deviceMemorySize() const { return _deviceMemorySize; }

	void getMemoryInfo(axRenderMemoryInfo& info);

private:
	VkPhysicalDevice _h = VK_NULL_HANDLE;
	VkInstance _inst = VK_NULL_HANDLE;
	VkPhysicalDeviceFeatures _features = {};

	VkPhysicalDeviceProperties _props = {};
	VkPhysicalDeviceMemoryProperties _memProps = {};

	axArray<VkExtensionProperties>		_extensionProps;
	axArray<VkQueueFamilyProperties>	_queueFamilyProps;

	axInt _index = 0;
	axInt _majorVersion = 0;
	axInt _minorVersion = 0;
	axInt _deviceMemorySize = 0;
};

class axCppVkPhysicalDeviceList : public axCppVkBase {
public:
	void create(VkInstance inst);
	axSpan<axCppVkPhysicalDevice>	devices()		{ return _devices; }
	axCppVkPhysicalDevice*			defaultDevice() { return &_devices[0]; }

private:
	axArray<axCppVkPhysicalDevice, 8>	_devices;
};

#endif
