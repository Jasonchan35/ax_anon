#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkSampler : public axCppVkBase {
public:
	const VkSampler& handle() const { return _h; }
	operator const VkSampler&() const { return _h; }

	axCppVkSampler() = default;
	axCppVkSampler(axCppVkSampler && r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
	}

	~axCppVkSampler() { destroy(); }

	void destroy();
	void create(VkDevice dev, const VkSamplerCreateInfo& info);

private:
	VkSampler _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif