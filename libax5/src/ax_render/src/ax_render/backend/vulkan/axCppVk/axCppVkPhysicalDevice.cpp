#if AX_USE_VULKAN

#include "axCppVkPhysicalDevice.h"

void axCppVkPhysicalDevice::create(axInt index_, VkInstance inst, VkPhysicalDevice dev) {
	_index = index_;
	_inst = inst;
	_h = dev;

	vkGetPhysicalDeviceProperties(_h, &_props);
	vkGetPhysicalDeviceFeatures(_h, &_features);

	{
		_deviceMemorySize = 0;
		vkGetPhysicalDeviceMemoryProperties(_h, &_memProps);
		for (uint32_t i = 0; i < _memProps.memoryHeapCount; i++) {
			auto& m = _memProps.memoryHeaps[i];
			if (m.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
				_deviceMemorySize += m.size;
			}
		}
	}

	VkResult err;
	{
		uint32_t count = 0;
		err = vkEnumerateDeviceExtensionProperties(_h, nullptr, &count, nullptr);
		Util::throwIfError(err);

		_extensionProps.resize(count);
		err = vkEnumerateDeviceExtensionProperties(_h, nullptr, &count, _extensionProps.data());
		Util::throwIfError(err);
	}

	_majorVersion = VK_VERSION_MAJOR(_props.apiVersion);
	_minorVersion = VK_VERSION_MINOR(_props.apiVersion);

	{	// get queueFamilyIndex
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_h, &count, nullptr);
		assert(count >= 1);

		_queueFamilyProps.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(_h, &count, _queueFamilyProps.data());
		assert(count >= 1);
	}

	logInfo();
}

void axCppVkPhysicalDevice::getMemoryInfo(axRenderMemoryInfo& info) {
	info = {};

	if (!hasExtension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
		return;

#if !AX_OS_OSX

	VkPhysicalDeviceMemoryBudgetPropertiesEXT budgetProp = {};
	budgetProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

	VkPhysicalDeviceMemoryProperties2 props2 = {};
	props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	props2.pNext = &budgetProp;

	vkGetPhysicalDeviceMemoryProperties2(_h, &props2);

	auto heapCount = props2.memoryProperties.memoryHeapCount;
	for (uint32_t i = 0; i < heapCount; i++) {
		auto& m = props2.memoryProperties.memoryHeaps[i];
		if (m.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
			info.budget += budgetProp.heapBudget[i];
			info.used   += budgetProp.heapUsage[i];
		}
	}
#endif
}

void axCppVkPhysicalDevice::logInfo() const {
	axTempString s;
	s.appendFormat(	"Vulkan Device[{?}]:\n"
					"  deviceName = {?}\n"
					"  deviceID = {?}\n"
					"  vendorID = {?}\n"
					"  driverVersion = {?}\n"
					"  majorVersion = {?}\n"
					"  minorVersion = {?}\n",
					_index,
					deviceName(),
					_props.deviceID,
					_props.vendorID,
					_props.driverVersion,
					_majorVersion,
					_minorVersion);
	s.append("  Extensions:");
	for (auto& e : _extensionProps) {
		s.append("    ", axStrView_c_str(e.extensionName), "\n");
	}

	AX_LOG("{?}", s);
}

bool axCppVkPhysicalDevice::hasExtension(const char* name) {
	for (auto& e : _extensionProps) {
		if (axCStr::equals(name, e.extensionName))
			return true;
	}
	return false;
}

bool axCppVkPhysicalDevice::findQueueFamilyIndex(uint32_t& outIndex, VkQueueFlags flags) const {
	uint32_t i = 0;
	for (auto& p : _queueFamilyProps) {
		if (p.queueFlags & flags) {
			outIndex = i;
			return true;
		}
		i++;
	}
	return false;
}

bool axCppVkPhysicalDevice::findComputeQueueFamilyIndex(uint32_t& outIndex) const {
	uint32_t i = 0;
	for (auto& p : _queueFamilyProps) {
		if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) // try to find non-graphic queue
			continue;

		if (p.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			outIndex = i;
			return true;
		}
		i++;
	}

	return findQueueFamilyIndex(outIndex, VK_QUEUE_COMPUTE_BIT);
}

bool axCppVkPhysicalDevice::findMemoryTypeIndex(uint32_t& outIndex, VkFlags typeBits, VkMemoryPropertyFlags requireMask) const {
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((_memProps.memoryTypes[i].propertyFlags & requireMask) == requireMask) {
				outIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

void axCppVkPhysicalDeviceList::create(VkInstance inst) {
	VkResult err;
	uint32_t count = 0;
	err = vkEnumeratePhysicalDevices(inst, &count, nullptr);
	Util::throwIfError(err);

	if (count <= 0) {
		AX_LOG("No vulkan physical device");
		throw axError_Undefined(AX_LOC);
	}

	axArray<VkPhysicalDevice, 8> tmp;

	tmp.resize(count);
	err = vkEnumeratePhysicalDevices(inst, &count, tmp.data());
	Util::throwIfError(err);

	_devices.resize(count);

	axInt i = 0;
	for (auto& dev : tmp) {
		_devices[i].create(i, inst, dev);
		i++;
	}
}

#endif
