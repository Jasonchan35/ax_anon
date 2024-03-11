#pragma once

#if AX_USE_VULKAN

#include "axCppVkImage.h"

class axCppVkImageView : public axCppVkBase {
public:
	const VkImageView& handle() { return _h; }
	operator const VkImageView&() { return _h; }

	axCppVkImageView() = default;
	axCppVkImageView(axCppVkImageView&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
	}

	~axCppVkImageView() { destroy(); }

	void destroy();
	void create(axCppVkImage& image);
	void create(VkDevice dev, VkImage image, VkFormat format);
	void create(VkDevice dev, VkImage image, VkImageViewCreateInfo& info);

private:
	VkImageView _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif