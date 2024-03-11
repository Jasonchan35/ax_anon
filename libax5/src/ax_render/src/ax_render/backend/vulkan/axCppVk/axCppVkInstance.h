#pragma once

#if AX_USE_VULKAN

#include "axCppVkBase.h"

struct axCppVkApplicationInfo : public VkApplicationInfo {
	axCppVkApplicationInfo();
};

class axCppVkInstance;

struct axCppVkInstanceCreateInfo : public VkInstanceCreateInfo {
	axCppVkApplicationInfo appInfo;
	axCppVkInstanceCreateInfo();

	struct Props : public axCppVkBase {
		Props();

		bool hasLayer(axStrView name) const;
		bool hasExtension(axStrView name) const;

		axArray<VkLayerProperties,		64>	layers;
		axArray<VkExtensionProperties,	64> extensions;
	};
	const Props	props;

	bool enableLayer		(axStrLiteral name);
	bool enableExtension	(axStrLiteral name);

	void logInfo();

friend class axCppVkInstance;
protected:
	axArray<const char*,	64> _enabledLayerNames;
	axArray<const char*,	64> _enabledExtensionNames;
};

class axCppVkInstance: public axCppVkBase {
public:
	const VkInstance& handle()		{ return _h; }
	operator const VkInstance&()	{ return _h; }

	~axCppVkInstance() { destroy(); }
	void destroy();

	void create(axCppVkInstanceCreateInfo& info);

private:
	VkInstance	_h = VK_NULL_HANDLE;
};

#endif