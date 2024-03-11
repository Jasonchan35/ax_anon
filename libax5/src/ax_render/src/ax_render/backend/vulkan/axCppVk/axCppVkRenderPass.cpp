#if AX_USE_VULKAN

#include "axCppVkRenderPass.h"

void axCppVkRenderPass::destroy() {
	if (_h) {
		vkDestroyRenderPass(_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

void axCppVkRenderPass::create(VkDevice dev, VkFormat colorFormat, VkFormat depthFormat, bool clear) {
	destroy();
	_dev = dev;

	bool hasDepth = depthFormat != VK_FORMAT_UNDEFINED;

	axArray<VkAttachmentDescription, 2> attachmentDescription;

	VkAttachmentReference colorReference = {};
	VkAttachmentReference depthReference = {};

	colorReference.attachment = Util::castUInt32(attachmentDescription.size());
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	{
		auto& dst = attachmentDescription.emplaceBack();
		dst.format			= colorFormat;
		dst.flags			= 0;
		dst.samples			= VK_SAMPLE_COUNT_1_BIT;
		dst.loadOp			= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		dst.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		dst.initialLayout	= clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
		dst.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	if (hasDepth) {
		depthReference.attachment = Util::castUInt32(attachmentDescription.size());
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		{
			auto& dst			= attachmentDescription.emplaceBack();
			dst.format			= depthFormat;
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.initialLayout	= clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ;
			dst.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}
		
	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.flags = 0;
	subpassDesc.inputAttachmentCount		= 0;
	subpassDesc.pInputAttachments			= nullptr;
	subpassDesc.colorAttachmentCount		= 1;
	subpassDesc.pColorAttachments			= &colorReference;
	subpassDesc.pResolveAttachments			= nullptr;
	subpassDesc.pDepthStencilAttachment		= hasDepth ? &depthReference : nullptr;
	subpassDesc.preserveAttachmentCount		= 0;
	subpassDesc.pPreserveAttachments		= nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext				= nullptr;
	renderPassCreateInfo.flags				= 0;
	renderPassCreateInfo.attachmentCount	= Util::castUInt32(attachmentDescription.size());
	renderPassCreateInfo.pAttachments		= attachmentDescription.data();
	renderPassCreateInfo.subpassCount		= 1;
	renderPassCreateInfo.pSubpasses			= &subpassDesc;
	renderPassCreateInfo.dependencyCount	= 0;
	renderPassCreateInfo.pDependencies		= nullptr;

	auto err = vkCreateRenderPass(dev, &renderPassCreateInfo, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}


#endif
