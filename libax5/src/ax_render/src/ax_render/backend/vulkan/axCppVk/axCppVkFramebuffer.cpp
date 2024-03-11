#if AX_USE_VULKAN

#include "axCppVkFramebuffer.h"

void axCppVkFramebuffer::destroy() {
	if (_h) {
		vkDestroyFramebuffer(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkFramebuffer::create(VkDevice dev, VkRenderPass renderPass, axSpan<VkImageView> attachments, const VkExtent2D& size) {
	destroy();
	_dev = dev;
	_size = size;

	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.renderPass = renderPass;
	info.attachmentCount = Util::castUInt32(attachments.size());
	info.pAttachments = attachments.data();
	info.width  = size.width;
	info.height = size.height;
	info.layers = 1;

	auto err = vkCreateFramebuffer(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

#endif
