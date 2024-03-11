#if AX_USE_VULKAN

#include "axCppVkDevice.h"

void axCppVkDevice::destroy() {
	if (_h) {
		vkDeviceWaitIdle(_h);
		vkDestroyDevice(_h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
	}
}

void axCppVkDevice::create(axCppVkPhysicalDevice& phyDev, const axVkQueueFamilyIndices& queueFamilyIndices) {
	destroy();
	_phyDev = &phyDev;

	float queuePriorities[1] = {0.0};

	axArray<VkDeviceQueueCreateInfo, 4> queuesCreateInfos;
	{
		auto& c = queuesCreateInfos.emplaceBack();
		c.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		c.pNext = nullptr;
		c.flags = 0;
		c.queueFamilyIndex = queueFamilyIndices.graphIndex();
		c.queueCount = 1;
		c.pQueuePriorities = queuePriorities;
	}

	if (queueFamilyIndices.presentQueueIsSeparated()) {
		auto& c = queuesCreateInfos.emplaceBack();
		c.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		c.pNext = nullptr;
		c.flags = 0;
		c.queueFamilyIndex = queueFamilyIndices.presentIndex();
		c.queueCount = 1;
		c.pQueuePriorities = queuePriorities;
	}

	for (axInt i = 1; i < queuesCreateInfos.size(); i++) {
		queuesCreateInfos[i-1].pNext = &queuesCreateInfos[i];
	}
	if (queuesCreateInfos.size()) {
		queuesCreateInfos.back().pNext = nullptr;
	}

#if 0
	_enabledFeatures = {};
	_enabledFeatures.largePoints				= phyDev.features().largePoints;
	_enabledFeatures.wideLines					= phyDev.features().wideLines;
	_enabledFeatures.fillModeNonSolid			= phyDev.features().fillModeNonSolid;
	//_enabledFeatures.fullDrawIndexUint32		= phyDev.features().fullDrawIndexUint32;
	//_enabledFeatures.robustBufferAccess		= phyDev.features().robustBufferAccess;
	_enabledFeatures.textureCompressionETC2		= phyDev.features().textureCompressionETC2;
	_enabledFeatures.textureCompressionASTC_LDR = phyDev.features().textureCompressionASTC_LDR;
	_enabledFeatures.textureCompressionBC		= phyDev.features().textureCompressionBC;
	//_enabledFeatures.multiViewport			= phyDev.features().multiViewport;
	//_enabledFeatures.alphaToOne				= 
	//_enabledFeatures.depthBounds				= 
	//_enabledFeatures.depthClamp				= 
	//_enabledFeatures.depthBiasClamp			= 
	_enabledFeatures.geometryShader				= phyDev.features().geometryShader;
	_enabledFeatures.tessellationShader			= phyDev.features().tessellationShader;
	_enabledFeatures.shaderFloat64				= phyDev.features().shaderFloat64;
#else
	_enabledFeatures = phyDev.features(); // enable all the hardware support
#endif

	axArray<const char*, 16> enabledExtensionNames;

	if (!_addEnabledExtensions(enabledExtensionNames, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) throw axError_Undefined(AX_LOC);

// optional extensions
	_addEnabledExtensions(enabledExtensionNames, VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
	// already included in Vulkan 1.1
	_addEnabledExtensions(enabledExtensionNames, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
	_addEnabledExtensions(enabledExtensionNames, VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType	= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext	= nullptr;
	deviceCreateInfo.flags	= 0;
	deviceCreateInfo.queueCreateInfoCount		= Util::castUInt32(queuesCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos			= queuesCreateInfos.data();
	deviceCreateInfo.enabledLayerCount			= 0;
	deviceCreateInfo.ppEnabledLayerNames		= nullptr;
	deviceCreateInfo.enabledExtensionCount		= Util::castUInt32(enabledExtensionNames.size());
	deviceCreateInfo.ppEnabledExtensionNames	= enabledExtensionNames.data();
	deviceCreateInfo.pEnabledFeatures			= &_enabledFeatures;

	auto err = vkCreateDevice(*_phyDev, &deviceCreateInfo, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

bool axCppVkDevice::_addEnabledExtensions(axIArray<const char*>& arr, const char* name) {
	if (!_phyDev->hasExtension(name))
		return false;
	arr.append(name);
	return true;
}

#endif
