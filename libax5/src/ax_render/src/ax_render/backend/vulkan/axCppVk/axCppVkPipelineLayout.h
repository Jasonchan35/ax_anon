#pragma once

#if AX_USE_VULKAN

#include "axCppVkDescriptorSetLayout.h"

class axCppVkPipelineLayout : public axCppVkBase {
public:
	const VkPipelineLayout& handle() { return _h; }
	operator const VkPipelineLayout&() { return _h; }
	~axCppVkPipelineLayout() { destroy(); }

	axCppVkPipelineLayout() = default;
	axCppVkPipelineLayout(axCppVkPipelineLayout&& r);

	void destroy();

	void create(axCppVkDevice& dev, axSpan<VkDescriptorSetLayout>	descriptorSets);

	axCppVkDevice& device() { return *_dev; }

private:
	VkPipelineLayout _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif