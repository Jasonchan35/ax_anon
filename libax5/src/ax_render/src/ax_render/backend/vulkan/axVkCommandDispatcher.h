#pragma once

#if AX_USE_VULKAN

#include "../../axRenderCommandDispatcher.h"
#include "axCppVk/axCppVk.h"
#include "axVkCommonBase.h"
#include "axVkGpuBuffer.h"
#include "axVkRenderContext_Base.h"
#include "axVkRenderer.h"
#include "axVkRenderRequest.h"

class axVkCommandDispatcher : public axRenderCommandDispatcherImpl<axVkCommandDispatcher> {
	using Base = axRenderCommandDispatcherImpl<axVkCommandDispatcher>;
public:
	using Util = axVkUtil;

	axVkCommandDispatcher(axVkRenderContext_Base* renderContext_, axVkRenderRequest* renderRequest_);

	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS

	axVkRenderContext_Base*			renderContext = nullptr;
	axCppVkDevice*					device = nullptr;
	VkRenderPass					renderPass = nullptr;
	axCppVkFramebuffer*				frameBuffer = nullptr;
	axCppVkCommandBuffer*			cmdBuffer = nullptr;
	axVkSwapChain*					swapChain = nullptr;
	axVkRenderRequest*				req = nullptr;
};

#endif