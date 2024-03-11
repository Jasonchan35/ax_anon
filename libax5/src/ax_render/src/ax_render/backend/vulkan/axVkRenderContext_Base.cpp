#if AX_USE_VULKAN

#include "axVkRenderer.h"
#include "axVkRenderContext_Base.h"
#include "axVkCommonBase.h"
#include "axVkTexture.h"
#include "axVkMaterial.h"
#include "axVkFence.h"
#include "axVkCommandDispatcher.h"

axVkRenderContext_Base::axVkRenderContext_Base(axVkRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
	, _lastVSync(true)
{
	_procList = renderer->procList();
}

void axVkRenderContext_Base::onPreDestroy() {
	ax_Vulkan_TODO
}

void axVkRenderContext_Base::_createDevice() {
//----
	onCreateVkSurface(_surface);
//----
	axVkQueueFamilyIndices queueFamilyIndices;
	queueFamilyIndices.create(_surface);
	auto* renderer_ = renderer();

	if (queueFamilyIndices != renderer_->queueFamilyIndices()) {
		throw axError_Undefined(AX_LOC);
	}

	auto& dev = Util::rootDevice();
	vkGetDeviceQueue(dev, queueFamilyIndices.graphIndex(),   0, &_graphQueue);
	vkGetDeviceQueue(dev, queueFamilyIndices.presentIndex(), 0, &_presentQueue);
}

void axVkRenderContext_Base::onDispatchCommands(axRenderRequest& req_) {
	auto* renderer_ = renderer();
	auto* req = ax_type_cast_debug<axVkRenderRequest>(&req_);
	auto& device = renderer_->device();

	axSPtr<axVkSwapChain> swapChain;
	{
		auto d = _mdata.scopedLock();
		if (!d->swapChain || d->swapChainDirty || _lastVSync != renderer_->vsync()) {
			d->swapChain = axSPtr_new(axVkSwapChain(*this, d->swapChain));
			d->swapChainDirty = false;
		}
		swapChain = d->swapChain;
	}

	req->resourceToKeep.swapChains.emplaceBack(swapChain);

	if (!swapChain->_swapChain) {
		throw axError_Undefined(AX_LOC);
	}

	req->resourceToKeep.clear();

	VkResult err;
	uint32_t imageIndex = 0;

	do {
		if (!swapChain) {
			throw axError_Undefined(AX_LOC);
		}
		// Get the index of the next available swapchain image:
		err = vkAcquireNextImageKHR(device,
									swapChain->_swapChain,
									UINT64_MAX,
									req->_imageAcquiredSemaphore,
									VK_NULL_HANDLE,
									&imageIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR) {
			// demo->swapchain is out of date (e.g. the window was resized) and
			// must be recreated:
			// _swapChainData.newObject(this);
			throw axError_Undefined(AX_LOC);


		} else if (err == VK_SUBOPTIMAL_KHR) {
			// demo->swapchain is not as optimal as it could be, but the platform's
			// presentation engine will still present the image correctly.
			break;
		} else {
			Util::throwIfError(err);
		}
	} while (err != VK_SUCCESS);

	swapChain->_gpuFrameIndex = imageIndex;

	auto& gpuFrame = swapChain->currentGpuFrame();
	req->_graphCmdBuffer.beginCommand();

//----------
	axVkCommandDispatcher dispatcher(this, req);
	dispatcher.renderPass		= renderer_->renderPass();
	dispatcher.swapChain		= swapChain;
	dispatcher.device			= &device;
	dispatcher.cmdBuffer		= &req->_graphCmdBuffer;
	dispatcher.frameBuffer		= &gpuFrame._frameBuffer;

	dispatcher.dispatch();
}

void axVkRenderContext_Base::cmdSwapBuffers(axVkCommandDispatcher& dispatcher) {
	auto* renderer_ = this->renderer();
	auto* req = dispatcher.req;

	auto& gpuFrame = dispatcher.swapChain->currentGpuFrame();
	req->_graphCmdBuffer.endRenderPass();
	req->_graphCmdBuffer.endCommand();

	auto* fence = ax_type_cast_debug<axVkFence>(dispatcher.req->completedFence());
	auto& queueFamilyIndices = renderer_->queueFamilyIndices();

	{
		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext					= nullptr;

		info.waitSemaphoreCount		= 1;
		info.pWaitSemaphores		= &dispatcher.req->_imageAcquiredSemaphore.handle();
		info.pWaitDstStageMask		= &waitDstStageMask;

		info.commandBufferCount		= 1;
		info.pCommandBuffers		= &req->_graphCmdBuffer.handle();

		info.signalSemaphoreCount	= 1;
		info.pSignalSemaphores		= &dispatcher.req->_drawCompleteSemaphore.handle();

		auto err = vkQueueSubmit(_graphQueue, 1, &info, fence->_fence);

		if (!Util::checkResult(err)) {
			uint32_t count = 0;
//			renderer_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count, nullptr);
			axArray<VkCheckpointDataNV, 128>	checkPoints;
			checkPoints.resize(count);

			for (auto& e : checkPoints) {
				e.sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;
				e.pNext = nullptr;
				e.pCheckpointMarker = nullptr;
				e.stage = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
			}

//			renderer_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count, checkPoints.data());

			for (auto& e : checkPoints.revEach()) {
				AX_LOG("checkPoint {?}", axStrView_c_str(static_cast<const char*>(e.pCheckpointMarker)));
			}
			ax_log_func_name();
		}
		Util::throwIfError(err);
	}

	if (queueFamilyIndices.presentQueueIsSeparated()) {
		// If we are using separate queues, change image ownership to the
		// present queue before presenting, waiting for the draw complete
		// semaphore and signalling the ownership released semaphore when finished

		req->_presentCmdBuffer.beginCommand();

		VkImageMemoryBarrier imageOwnershipBarrier = {};		
		imageOwnershipBarrier.sType								= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageOwnershipBarrier.pNext								= nullptr;
		imageOwnershipBarrier.srcAccessMask						= 0;
		imageOwnershipBarrier.dstAccessMask						= 0;
		imageOwnershipBarrier.oldLayout							= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageOwnershipBarrier.newLayout							= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageOwnershipBarrier.srcQueueFamilyIndex				= queueFamilyIndices.graphIndex();
		imageOwnershipBarrier.dstQueueFamilyIndex				= queueFamilyIndices.presentIndex();
		imageOwnershipBarrier.image								= gpuFrame._colorImage;
		imageOwnershipBarrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		imageOwnershipBarrier.subresourceRange.baseMipLevel		= 0;
		imageOwnershipBarrier.subresourceRange.levelCount		= 1;
		imageOwnershipBarrier.subresourceRange.baseArrayLayer	= 0;
		imageOwnershipBarrier.subresourceRange.layerCount		= 1;

		vkCmdPipelineBarrier(	req->_presentCmdBuffer,
								VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								0, 
								0, nullptr, // memory barrier
								0, nullptr, // buffer memory barrier
								1, &imageOwnershipBarrier);

		req->_presentCmdBuffer.endCommand();

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext					= nullptr;
		info.waitSemaphoreCount		= 1;
		info.pWaitSemaphores		= &dispatcher.req->_drawCompleteSemaphore.handle();
		info.pWaitDstStageMask		= &waitDstStageMask;

		info.commandBufferCount		= 1;
		info.pCommandBuffers		= &req->_presentCmdBuffer.handle();

		info.signalSemaphoreCount	= 1;
		info.pSignalSemaphores		= &dispatcher.req->_imageOwnershipSemaphore.handle();

		auto err = vkQueueSubmit(_presentQueue, 1, &info, VK_NULL_HANDLE);
		Util::throwIfError(err);
	}

	{
		uint32_t imageIndex = Util::castUInt32(dispatcher.swapChain->_gpuFrameIndex);
		VkPresentInfoKHR info = {};
		info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext				= nullptr;

		info.waitSemaphoreCount = 1;
		if (queueFamilyIndices.presentQueueIsSeparated()) {
			 // If we are using separate queues we have to wait for image ownership
			info.pWaitSemaphores = &dispatcher.req->_imageOwnershipSemaphore.handle();
		} else {
			info.pWaitSemaphores = &dispatcher.req->_drawCompleteSemaphore.handle();
		}

		info.swapchainCount		= 1;
		info.pSwapchains		= &dispatcher.swapChain->_swapChain.handle();
		info.pImageIndices		= &imageIndex;
		info.pResults			= nullptr;

		auto err = vkQueuePresentKHR(_graphQueue, &info);

		if (err == VK_ERROR_OUT_OF_DATE_KHR) {
			// swapchain is out of date (e.g. the window was resized) and
			// must be recreated:
			auto d = _mdata.scopedLock();
			d->swapChainDirty = true;

		} else if (err == VK_SUBOPTIMAL_KHR) {
			// swapchain is not as optimal as it could be, but the platform's
			// presentation engine will still present the image correctly.
		} else {
			Util::throwIfError(err);
		}
	}
}

void axVkRenderContext_Base::onSetNativeViewRect(const axRect2f& rect) {
	{
		auto d = _mdata.scopedLock();
		d->swapChainDirty = true;
	}
	Base::onSetNativeViewRect(rect);
}

#endif //AX_USE_VULKAN
