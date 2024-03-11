#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkPipelineCache : public axCppVkBase {
public:
	const VkPipelineCache& handle() { return _h; }
	operator const VkPipelineCache&() { return _h; }

	axCppVkPipelineCache() = default;
	axCppVkPipelineCache(axCppVkPipelineCache && r);

	~axCppVkPipelineCache() { destroy(); }

	void destroy();
	void create(axCppVkDevice& dev);
	axCppVkDevice& device() { return *_dev; }

private:
	VkPipelineCache _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif