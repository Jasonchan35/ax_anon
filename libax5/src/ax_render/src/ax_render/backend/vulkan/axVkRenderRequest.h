#pragma once

#include "../../axRenderRequest.h"
#include "axVkCommonBase.h"

class axVkRenderRequest : public axRenderRequest {
	AX_RTTI_CLASS(axVkRenderRequest, axRenderRequest)
public:
	using Util = axVkUtil;
	
	axVkRenderRequest() {
		auto& dev = Util::rootDevice();
		auto& qi = Util::renderer()->queueFamilyIndices();

		_drawCompleteSemaphore.create(dev);
		_imageAcquiredSemaphore.create(dev);
		_graphCmdPool.create(dev, qi.graphIndex());
		_graphCmdBuffer.create(_graphCmdPool);

		if (qi.presentQueueIsSeparated()) {
			_imageOwnershipSemaphore.create(dev);
			_presentCmdPool.create(dev, qi.presentIndex());
			_presentCmdBuffer.create(_presentCmdPool);
		}
	}

	virtual void onGpuEnded() override {
		resourceToKeep.clear();
	}

	class ResourceToKeep {
	public:
		ResourceToKeep() {
			vkMaterialPasses.reserve(128);
			stagingBuffers.reserve(128);
			vkImageViews.reserve(128);
			vkImages.reserve(128);
			vkGpuBuffers.reserve(128);
			textures.reserve(128);
		}

		void clear() {
			vkMaterialPasses.clear();
			stagingBuffers.clear();
			vkImageViews.clear();
			vkImages.clear();
			vkGpuBuffers.clear();
			textures.clear();
			swapChains.clear();
		}

		axArray<axVkStagingBuffer>		stagingBuffers;
		axArray<axCppVkImage>			vkImages;
		axArray<axCppVkImageView>		vkImageViews;
		axSPtrArray<axVkMaterialPass>	vkMaterialPasses;
		axSPtrArray<axVkGpuBuffer>		vkGpuBuffers;
		axSPtrArray<axTexture>			textures;
		axSPtrArray<axVkSwapChain>		swapChains;
	};

	ResourceToKeep	resourceToKeep;

	axCppVkSemaphore	_drawCompleteSemaphore;
	axCppVkSemaphore	_imageAcquiredSemaphore;
	axCppVkSemaphore	_imageOwnershipSemaphore;

	axCppVkCommandPool		_graphCmdPool;
	axCppVkCommandPool		_presentCmdPool;

	axCppVkCommandBuffer	_graphCmdBuffer;
	axCppVkCommandBuffer	_presentCmdBuffer;
};
