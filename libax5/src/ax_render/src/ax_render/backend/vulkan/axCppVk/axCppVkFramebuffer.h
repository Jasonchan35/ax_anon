#pragma once

#if AX_USE_VULKAN

#include "axCppVkBase.h"

class axCppVkFramebuffer : public axCppVkBase {
public:
	const VkFramebuffer& handle() { return _h; }
	operator const VkFramebuffer&() { return _h; }

	axCppVkFramebuffer() = default;
	axCppVkFramebuffer(axCppVkFramebuffer&& r) {
		_h = r._h;
		_dev = r._dev;
		_size = r._size;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
		r._size = VkExtent2D();
	}

	~axCppVkFramebuffer() { destroy(); }

	void destroy();
	void create(VkDevice dev, VkRenderPass renderPass, axSpan<VkImageView> attachments, const VkExtent2D& size);

	const VkExtent2D& size() const { return _size; }

private:
	VkFramebuffer _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
	VkExtent2D _size = {};
};

#endif