#pragma once

#if AX_USE_DX11

#include "../../axRenderCommandDispatcher.h"
#include "axDX11CommonBase.h"

class axDX11CommandDispatcher : public axRenderCommandDispatcherImpl<axDX11CommandDispatcher> {
	AX_RTTI_CLASS(axDX11CommandDispatcher, axRenderCommandDispatcherImpl<axDX11CommandDispatcher>)
public:
	using Util = axDX11Util;

	axDX11CommandDispatcher(axDX11RenderContext* renderContext_, axRenderRequest* renderRequest_);

	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS;

	axDX11RenderContext*		renderContext = nullptr;
	axDX11_ID3DDevice*			d3dDevice = nullptr;
	ID3D11DeviceContext*		d3dContext = nullptr;
	ID3D11RenderTargetView*		_renderTargetView = nullptr;
	ID3D11DepthStencilView*		_depthStencilView = nullptr;
	axRenderRequest*			renderRequest = nullptr;
private:
	axArray< axDX11Texture2D* >		_uploadedTextures;
};

#endif