#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkRenderPass : public axCppVkBase {
public:
	using Util = axVkUtil;

	const VkRenderPass& handle() { return _h; }
	operator const VkRenderPass&() { return _h; }

	axCppVkRenderPass() = default;
	axCppVkRenderPass(axCppVkRenderPass&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
	}

	~axCppVkRenderPass() { destroy(); }

	void destroy();
	void create(VkDevice dev, VkFormat colorFormat, VkFormat depthFormat, bool clear);

private:
	VkRenderPass _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif