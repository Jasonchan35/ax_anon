#pragma once

#include "../../axRenderer.h"

#if AX_USE_DX12

#include "axDX12CommonBase.h"

class axDX12RayTracing;

class axDX12Renderer : public axRenderer {
	AX_RTTI_CLASS(axDX12Renderer, axRenderer);
public:
	ax_DOWNCAST_GET_INSTANCE();

	using Util = axDX12Util;
	axDX12Renderer(CreateDesc& desc);

	ID3D12Device*		d3dDevice	()	{ return _d3dDevice; }
	IDXGIFactory4*		dxgiFactory	()	{ return _dxgiFactory; }

#if _DEBUG
	IDXGIDebug1*		dxgiDebug	()	{ return _dxgiDebug; }
#endif

protected:
	axRenderer_InterfaceFunctions(axDX12);

	virtual void onGetMemoryInfo(MemoryInfo& info) override;

private:
	void _getHardwareAdpter();

	D3D_FEATURE_LEVEL				_d3dFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	axComPtr<axDX12_IDXGIFactory>	_dxgiFactory;
	axComPtr<axDX12_ID3D12Device>	_d3dDevice;
	axComPtr<axDX12_IDXGIAdapter>	_dxgiAdapter;

#if _DEBUG
	axComPtr<axDX12_IDXGIDebug>		_dxgiDebug;
	axComPtr<axDX12_ID3D12Debug>	_d3dDebug;
#endif
};

#endif // AX_USE_DX12