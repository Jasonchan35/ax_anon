#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"
#include "axCppVkShaderModule.h"
#include "axCppVkPipelineCache.h"
#include "axCppVkPipelineLayout.h"

class axCppVkPipeline;

class axCppVkPipeline : public axCppVkBase {
public:
	const VkPipeline& handle() { return _h; }
	operator const VkPipeline&() { return _h; }

	axCppVkPipeline() = default;
	axCppVkPipeline(axCppVkPipeline && r);

	~axCppVkPipeline() { destroy(); }

	void destroy();
	void create(axCppVkDevice& dev, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& info);
	void create(axCppVkDevice& dev, VkPipelineCache cache, const VkComputePipelineCreateInfo&  info);

	axCppVkDevice& device() { return *_dev; }

private:
	VkPipeline _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif