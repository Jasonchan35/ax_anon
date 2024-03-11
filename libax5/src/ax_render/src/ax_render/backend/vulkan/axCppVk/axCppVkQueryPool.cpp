#if AX_USE_VULKAN

#include "axCppVkQueryPool.h"
#include "axCppVkQueryPool.h"

axCppVkQueryPool::axCppVkQueryPool(axCppVkQueryPool&& r) {
	_h	   = r._h;
	_dev   = r._dev;
	r._h   = VK_NULL_HANDLE;
	r._dev = VK_NULL_HANDLE;
}

void axCppVkQueryPool::destroy() {
	if (_h) {
		vkDestroyQueryPool(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkQueryPool::create(VkDevice dev, VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics, VkQueryPoolCreateFlags flags) {
	destroy();
	_dev = dev;

	VkQueryPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	info.queryType = queryType;
	info.queryCount = queryCount;
	info.pipelineStatistics = pipelineStatistics;

	auto res = vkCreateQueryPool(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(res);
}

#endif