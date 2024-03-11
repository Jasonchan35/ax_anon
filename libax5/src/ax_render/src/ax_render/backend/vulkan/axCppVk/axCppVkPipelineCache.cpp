#if AX_USE_VULKAN

#include "axCppVkPipelineCache.h"

axCppVkPipelineCache::axCppVkPipelineCache(axCppVkPipelineCache&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void axCppVkPipelineCache::destroy() {
	if (_h) {
		vkDestroyPipelineCache(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkPipelineCache::create(axCppVkDevice& dev) {
	destroy();
	_dev = &dev;

	VkPipelineCacheCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.initialDataSize = 0;
	info.pInitialData = nullptr;

	auto err = vkCreatePipelineCache(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif