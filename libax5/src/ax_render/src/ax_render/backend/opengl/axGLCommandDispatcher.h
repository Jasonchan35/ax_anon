#pragma once

#if AX_USE_OPENGL

#include "../../axRenderCommandDispatcher.h"
#include "axGLCommonBase.h"

class axGLCommandDispatcher : public axRenderCommandDispatcherImpl<axGLCommandDispatcher> {
	using Base = axRenderCommandDispatcherImpl<axGLCommandDispatcher>;
public:
	using Util = axGLUtil;

	axGLCommandDispatcher(axGLRenderContext_Base* renderContext_, axRenderRequest* renderRequest_);

	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS

	axGLRenderContext_Base*		renderContext = nullptr;

private:

	class BindShaderPass : public axNonCopyable {
	public:
		BindShaderPass(axGLCommandDispatcher& dispatcher, axRenderDrawCall& drawCall_, axGLMaterial* mtl);
		~BindShaderPass();

		axGLCommandDispatcher*	dispatcher = nullptr;
		axRenderDrawCall&		drawCall;
		axGLMaterial*			material = nullptr;
	};

	axArray< axGLTexture2D* >	_uploadedTextures;
	axRenderRequest*			renderRequest = nullptr;
};

#endif