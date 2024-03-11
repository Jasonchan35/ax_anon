#pragma once

#include <ax_render/ax_render_config.h>

#if AX_USE_DX11

#include <ax_render/backend/dx11/axDX11CommonBase.h>
#include <ax_render/texture/axTexture.h>

class axDX11Renderer;
class axRenderContext;

class axDX11Texture2D : public axTexture2D {
	AX_RTTI_CLASS(axDX11Texture2D, axTexture2D);
public:
	using Util = axDX11Util;

	axDX11Texture2D(CreateDesc& desc);

	void onUploadRequest(axDX11CommandDispatcher& dispatcher, UploadRequest& req);

	axComPtr<ID3D11Texture2D>			_tex;
	axComPtr<ID3D11SamplerState>		_samplerState;
	axComPtr<ID3D11ShaderResourceView>	_resourceView;
};

#endif //AX_USE_DX11
