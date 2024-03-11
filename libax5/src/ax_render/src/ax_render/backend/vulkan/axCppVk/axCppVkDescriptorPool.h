#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"
#include "axCppVkDescriptorSetLayout.h"

class axCppVkDescriptorPool : public axCppVkBase {
public:
	const VkDescriptorPool& handle() { return _h; }
	operator const VkDescriptorPool&() { return _h; }
	~axCppVkDescriptorPool() { destroy(); }

	axCppVkDescriptorPool() = default;
	axCppVkDescriptorPool(axCppVkDescriptorPool&& r);

	void destroy();
	void create(axCppVkDevice& dev, axSpan<VkDescriptorPoolSize> poolSizes, axInt maxSets);

	VkDescriptorSet allocDescriptorSet(VkDescriptorSetLayout descSetLayout);
	void allocDescriptorSets(axIArray<VkDescriptorSet>& outSets, axInt count, VkDescriptorSetLayout descSetLayout);

	void resetPool() {
		vkResetDescriptorPool(*_dev, _h, 0);
	}

	axCppVkDevice& device() { return *_dev; }

private:
	VkDescriptorPool  _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
};

#endif