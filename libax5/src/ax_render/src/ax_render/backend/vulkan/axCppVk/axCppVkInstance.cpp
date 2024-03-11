#if AX_USE_VULKAN

#include "axCppVkInstance.h"

void axCppVkInstance::destroy() {
	if (_h) {
		vkDestroyInstance(_h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
	}
}

void axCppVkInstance::create(axCppVkInstanceCreateInfo& info) {
	destroy();

	VkResult err;
	err = vkEnumerateInstanceVersion(&info.appInfo.apiVersion);
	Util::throwIfError(err);


	info.enabledLayerCount			= Util::castUInt32(info._enabledLayerNames.size());
	info.ppEnabledLayerNames		= info._enabledLayerNames.data();

	info.enabledExtensionCount		= Util::castUInt32(info._enabledExtensionNames.size());
	info.ppEnabledExtensionNames	= info._enabledExtensionNames.data();

	err = vkCreateInstance(&info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

axCppVkInstanceCreateInfo::Props::Props() {
	VkResult err;
	uint32_t count = 0;
	{
		err = vkEnumerateInstanceLayerProperties(&count, nullptr);
		Util::throwIfError(err);

		layers.resize(count);
		err = vkEnumerateInstanceLayerProperties(&count, layers.data());
		Util::throwIfError(err);
	}

	{
		err = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		Util::throwIfError(err);

		extensions.resize(count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
		Util::throwIfError(err);
	}
}

bool axCppVkInstanceCreateInfo::Props::hasLayer(axStrView name) const {
	for (auto& e : layers) {
		if (axStrView_c_str(e.layerName) == name)
			return true;
	}
	return false;
}

bool axCppVkInstanceCreateInfo::Props::hasExtension(axStrView name) const {
	for (auto& e : extensions) {
		if (axStrView_c_str(e.extensionName) == name)
			return true;
	}
	return false;
}

bool axCppVkInstanceCreateInfo::enableLayer(axStrLiteral name) {
	if (!props.hasLayer(name))
		return false;
	_enabledLayerNames.append(name.c_str());
	return true;
}

bool axCppVkInstanceCreateInfo::enableExtension(axStrLiteral name) {
	if (!props.hasExtension(name))
		return false;
	_enabledExtensionNames.append(name.c_str());
	return true;
}

void axCppVkInstanceCreateInfo::logInfo() {
	axTempString s;
	s.append("VkInstance Layers\n");
	for (auto& e : props.layers) {
		s.appendFormat("  {?,40} spec={?}, impl={?,3}, desc={?}\n",
			axStrView_c_str(e.layerName),
			e.specVersion,
			e.implementationVersion,
			axStrView_c_str(e.description));
	}
	s.append("VkInstance Extensions\n");
	for (auto& e : props.extensions) {
		s.appendFormat("  {?,40} spec={?}\n",
			axStrView_c_str(e.extensionName),
			e.specVersion);
	}
	AX_LOG("{?}", s);
}

axCppVkApplicationInfo::axCppVkApplicationInfo() {
	sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	pNext = nullptr;
	pApplicationName = "axVkRenderer";
	applicationVersion = 0;
	pEngineName = "axVkRenderer";
	engineVersion = 0;
	apiVersion = VK_MAKE_VERSION(1, 1, 0);
}

axCppVkInstanceCreateInfo::axCppVkInstanceCreateInfo() {
	sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	pNext = nullptr;
	flags = 0;
	pApplicationInfo = &appInfo;
	enabledLayerCount = 0;
	ppEnabledLayerNames = nullptr;
	enabledExtensionCount = 0;
	ppEnabledExtensionNames = nullptr;
}

#endif