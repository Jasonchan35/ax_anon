#pragma once

#if AX_USE_METAL

#include "../../axRenderCommandDispatcher.h"
#include "axMTLCommonBase.h"

class axMTLCommandDispatcher : public axRenderCommandDispatcherImpl<axMTLCommandDispatcher> {
	using Base = axRenderCommandDispatcherImpl<axMTLCommandDispatcher>;
public:
	using Util = axMTLUtil;

	axMTLCommandDispatcher(axMTLRenderContext_Base* renderContext_, axRenderRequest* renderReq);

	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS

	axMTLRenderContext_Base*		renderContext = nullptr;
	MTKView*						mtkView = nullptr;

	id<MTLRenderCommandEncoder>		cmdEncoder = nil;
	id<MTLComputeCommandEncoder>	computeCmdEncoder = nil;
	id<MTLBlitCommandEncoder>		blitCmdEncoder = nil;
};

#endif
