#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkShaderModule : public axCppVkBase {
public:
	const VkShaderModule& handle() { return _h; }
	operator const VkShaderModule&() { return _h; }

	axCppVkShaderModule() = default;
	axCppVkShaderModule(axCppVkShaderModule&& r);

	~axCppVkShaderModule() { destroy(); }

	void destroy();
	void create			(axCppVkDevice& dev, axByteSpan byteCode);
	void createFromFile	(axCppVkDevice& dev, axStrView  filename);

	axCppVkDevice& device() { return *_dev; }

private:
	VkShaderModule _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif