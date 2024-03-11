#if AX_USE_VULKAN

#include "axVkSwapChain.h"
#include "axVkRenderContext.h"

axVkSwapChain::axVkSwapChain(axVkRenderContext_Base& ctx, axVkSwapChain* oldSwapChain) {
	auto* renderer  = axVkRenderer::s_instance();
	auto* device	= &renderer->device();
	auto& queueFamilyIndices	= renderer->queueFamilyIndices();
	auto& surface = ctx.surface();

	if (!surface)
		throw axError_Undefined(AX_LOC);

	auto cap = surface.getCapabilities();
	if (cap.currentExtent.width == 0 || cap.currentExtent.height == 0)
		throw axError_Undefined(AX_LOC);

	VkPresentModeKHR	presentMode	= renderer->vsync() ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

	VkSurfaceFormatKHR	colorFormat;
	colorFormat.format		= VK_FORMAT_B8G8R8A8_UNORM;
	colorFormat.colorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkFormat	depthStencilFormat	= VK_FORMAT_D32_SFLOAT;

	_swapChain.create(*device, surface, 
						oldSwapChain ? &oldSwapChain->_swapChain : nullptr,
						colorFormat,
						Util::castUInt32(axRender_kMaxFrameAheadCount),
						presentMode, queueFamilyIndices);

	bool hasDepth = depthStencilFormat != VK_FORMAT_UNDEFINED;

	if (hasDepth) {
		_depthImage.createDepthStencil(	*device,
										cap.currentExtent.width,
										cap.currentExtent.height,
										depthStencilFormat);

		_depthMemory.createForImage(_depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		_depthImageView.create(_depthImage);
	}

	{	// init FrameData
		axArray<VkImage, 4> images;
		_swapChain.getImages(images);

		_gpuFrames.resize(images.size());

		axInt i = 0;
		for (auto& frameData : _gpuFrames) {
			auto& colorImage = images[i];
			frameData._colorImage = colorImage;
			frameData._colorImageView.create(*device, colorImage, colorFormat.format);

			axArray<VkImageView, 2> attachments;
			attachments.clear();
			attachments.emplaceBack(frameData._colorImageView);

			if (hasDepth) {
				attachments.emplaceBack(_depthImageView);
			}

			frameData._frameBuffer.create(*device, renderer->renderPass(), attachments, cap.currentExtent);
			i++;
		}
	}
}

axVkSwapChain::~axVkSwapChain() {
	vkDeviceWaitIdle(Util::renderer()->device());
}

#endif
