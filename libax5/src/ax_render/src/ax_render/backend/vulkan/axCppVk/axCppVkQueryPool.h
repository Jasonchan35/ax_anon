#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkQueryPool : public axCppVkBase {
public:
	const VkQueryPool& handle() { return _h; }
	operator const VkQueryPool&() { return _h; }

	axCppVkQueryPool() = default;
	axCppVkQueryPool(axCppVkQueryPool&& r);

	~axCppVkQueryPool() { destroy(); }

	void destroy();
	void create(VkDevice dev, VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics = 0, VkQueryPoolCreateFlags flags = 0);
	
	VkDevice device() { return _dev; }

private:
	VkQueryPool _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif