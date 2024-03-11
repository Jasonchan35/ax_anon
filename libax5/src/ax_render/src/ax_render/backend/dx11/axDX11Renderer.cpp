#if AX_USE_DX11

#include "axDX11Renderer.h"
#include "axDX11RenderContext.h"
#include "axDX11StorageBuffer.h"
#include "axDX11StageBuffer.h"
#include "axDX11Shader.h"
#include "axDX11Material.h"
#include "axDX11MaterialPass.h"
#include "axDX11Texture.h"
#include "axDX11RenderRequest.h"

#include "../../shader/axShader.h"

axRenderer_InterfaceFunctions_Impl(axDX11)

axDX11Renderer::axDX11Renderer(CreateDesc& desc) 
	: Base(desc)
{
	D3D_FEATURE_LEVEL featureLevel;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	axComPtr<ID3D11Device>			d3dDevice;
	axComPtr<ID3D11DeviceContext>	d3dContext;


	HRESULT hr;
	hr = D3D11CreateDevice(	nullptr,
							D3D_DRIVER_TYPE_HARDWARE,
							nullptr,
							createDeviceFlags,
							nullptr,
							0, 
							D3D11_SDK_VERSION,
							d3dDevice.ptrForInit(), 
							&featureLevel,
							d3dContext.ptrForInit());
	Util::throwIfError(hr);
	//This method returns DXGI_ERROR_SDK_COMPONENT_MISSING if you specify D3D11_CREATE_DEVICE_DEBUG in Flags
	//and the incorrect version of the debug layer is installed on your computer. Install the latest Windows SDK to get the correct version.

	hr = d3dDevice->QueryInterface(IID_PPV_ARGS(_d3dDevice.ptrForInit()));
	Util::throwIfError(hr);

	hr = d3dContext->QueryInterface(IID_PPV_ARGS(_d3dContext.ptrForInit()));
	Util::throwIfError(hr);

	hr = _d3dDevice->QueryInterface(IID_PPV_ARGS(_dxgiDevice.ptrForInit()));
	Util::throwIfError(hr);

	{
		axComPtr<IDXGIAdapter> adapter;
		hr = _dxgiDevice->GetAdapter(adapter.ptrForInit());
		Util::throwIfError(hr);

		hr = adapter->QueryInterface(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		Util::throwIfError(hr);
	}

	{
		D3D11_FEATURE_DATA_THREADING s = {};
		hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &s, sizeof(s));
		Util::throwIfError(hr);
		_adapterInfo.multithread = s.DriverConcurrentCreates && s.DriverCommandLists;
		if (!_adapterInfo.multithread)
			desc.multithread = false;
	}

	//{
	//	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS s = {};
	//	hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &s, sizeof(s));
	//	Util::throwIfError(hr);
	//	_adapterInfo.computeShader = s.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
	//}
	_adapterInfo.computeShader = true;

	{
		D3D11_FEATURE_DATA_DOUBLES s = {};
		hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &s, sizeof(s));
		Util::throwIfError(hr);
		_adapterInfo.shaderFloat64 = s.DoublePrecisionFloatShaderOps;
	}

	{
		DXGI_ADAPTER_DESC ad;
		hr = _dxgiAdapter->GetDesc(&ad);
		Util::throwIfError(hr);

		_adapterInfo.adapterName.setUtf(axStrView_c_str(ad.Description));
		_adapterInfo.memorySize = ad.DedicatedVideoMemory;
	}

	hr = _dxgiAdapter->GetParent(IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
	Util::throwIfError(hr);

	if (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) {
		hr = _d3dDevice->QueryInterface(_d3dDebug.ptrForInit());
		Util::throwIfError(hr);
	}
}

axDX11Renderer::~axDX11Renderer() {
	onDestroy();
}

void axDX11Renderer::validateContext() {
	if (!_d3dDebug) return;
	auto hr = _d3dDebug->ValidateContext(_d3dContext.ptr());
	Util::throwIfError(hr);
}

void axDX11Renderer::onGetMemoryInfo(MemoryInfo& info) {
	DXGI_QUERY_VIDEO_MEMORY_INFO  i;
	auto hr = _dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &i);
	Util::throwIfError(hr);

	info.used = i.CurrentUsage;
	info.budget = i.Budget;
}

#endif //AX_USE_DX11