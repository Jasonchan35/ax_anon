#if AX_USE_VULKAN

#include "axCppVkDescriptorPool.h"

axCppVkDescriptorPool::axCppVkDescriptorPool(axCppVkDescriptorPool&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void axCppVkDescriptorPool::destroy() {
	if (_h) {
		vkDestroyDescriptorPool(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkDescriptorPool::create(axCppVkDevice& dev, axSpan<VkDescriptorPoolSize> poolSizes, axInt maxSets) {
	destroy();
	_dev = &dev;

	VkDescriptorPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.maxSets = Util::castUInt32(maxSets);
	info.poolSizeCount = Util::castUInt32(poolSizes.size());
	info.pPoolSizes = poolSizes.data();

	auto err = vkCreateDescriptorPool(*_dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

VkDescriptorSet axCppVkDescriptorPool::allocDescriptorSet(VkDescriptorSetLayout descSetLayout) {
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorPool = _h;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &descSetLayout;

	VkDescriptorSet outDescSet = nullptr;
	vkAllocateDescriptorSets(*_dev, &info, &outDescSet);
	return outDescSet;
}

void axCppVkDescriptorPool::allocDescriptorSets(axIArray<VkDescriptorSet>& outSets, axInt count, VkDescriptorSetLayout descSetLayout) {
	axArray<VkDescriptorSetLayout, 8> descSetLayoutArray;
	descSetLayoutArray.resize(count, descSetLayout);

	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorPool = _h;
	info.descriptorSetCount = Util::castUInt32(descSetLayoutArray.size());
	info.pSetLayouts = descSetLayoutArray.data();

	outSets.resize(count);
	vkAllocateDescriptorSets(*_dev, &info, outSets.data());
}


#endif