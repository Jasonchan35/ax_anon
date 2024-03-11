#pragma once

#include "../../axRenderer.h"
#include "axDX11CommonBase.h"

#if AX_USE_DX11

class axDX11RenderContext;
class axDX11Shader;

class axDX11Renderer : public axRenderer {
	AX_RTTI_CLASS(axDX11Renderer, axRenderer);
public:
	ax_DOWNCAST_GET_INSTANCE();

	using Util = axDX11Util;

	axDX11Renderer(CreateDesc& desc);
	~axDX11Renderer();

	axDX11_ID3DDevice*			d3dDevice	() { return _d3dDevice; }
	axDX11_ID3DDeviceContext*	d3dContext	() { return _d3dContext; }
	axDX11_IDXGIFactory*		dxgiFactory	() { return _dxgiFactory; }
	axDX11_ID3D11Debug*			d3dDebug	() { return _d3dDebug; }
	axDX11_IDXGIDevice*			dxgiDevice	() { return _dxgiDevice; }
	axDX11_IDXGIAdapter*		dxgiAdapter	() { return _dxgiAdapter; }

	void validateContext();

protected:
	axRenderer_InterfaceFunctions(axDX11);

	virtual void onGetMemoryInfo(MemoryInfo& info) override;

	axComPtr<axDX11_ID3DDevice>			_d3dDevice;
	axComPtr<axDX11_ID3DDeviceContext>	_d3dContext;
	axComPtr<axDX11_IDXGIFactory>		_dxgiFactory;
	axComPtr<axDX11_IDXGIDevice>		_dxgiDevice;
	axComPtr<axDX11_IDXGIAdapter>		_dxgiAdapter;
	axComPtr<axDX11_ID3D11Debug>		_d3dDebug;

//	void _createVertexInputLayout(axComPtr<ID3D11InputLayout>& outLayout, VertexType vertexType, axByteSpan vertexShaderByteCode);
};

#endif //AX_USE_DX11
