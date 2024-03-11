#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkDescriptorSetLayout : public axCppVkBase {
public:
	const VkDescriptorSetLayout& handle() { return _h; }
	operator const VkDescriptorSetLayout&() { return _h; }
	~axCppVkDescriptorSetLayout() { destroy(); }

	axCppVkDescriptorSetLayout() = default;
	axCppVkDescriptorSetLayout(axCppVkDescriptorSetLayout&& r);

	void destroy();
	void create(axCppVkDevice& dev, axSpan<VkDescriptorSetLayoutBinding> bindings);

	axCppVkDevice& device() { return *_dev; }

private:
	VkDescriptorSetLayout _h = nullptr;
	axCppVkDevice* _dev = nullptr;
};

#endif