#include "../../ax_render_config.h"

#if AX_USE_DX12

#include "axDX12Renderer.h"
#include "axDX12RenderContext.h"
#include "axDX12StorageBuffer.h"
#include "axDX12StageBuffer.h"
#include "axDX12Shader.h"
#include "axDX12Material.h"
#include "axDX12Texture.h"
#include "axDX12RayTracing.h"
#include "axDX12Fence.h"
#include "axDX12RenderRequest.h"

axRenderer_InterfaceFunctions_Impl(axDX12)

axDX12Renderer::axDX12Renderer(CreateDesc& desc)
	: Base(desc)
{
	HRESULT hr;
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(_dxgiDebug.ptrForInit()));
	Util::throwIfError(hr);

	hr = D3D12GetDebugInterface(IID_PPV_ARGS(_d3dDebug.ptrForInit()));
	Util::throwIfError(hr);

	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	_d3dDebug->EnableDebugLayer();
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif
	
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
	Util::throwIfError(hr);

	_getHardwareAdpter();

	hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(_d3dDevice.ptrForInit())); 
	Util::throwIfError(hr);

	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		ARRAYSIZE(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	Util::throwIfError(hr);

	_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;

	{
		D3D12_FEATURE_DATA_SHADER_MODEL s = {};
		auto reqShaderModel = D3D_SHADER_MODEL_6_0;
		s.HighestShaderModel = reqShaderModel;
		hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &s, sizeof(s));
		Util::throwIfError(hr);

		if (s.HighestShaderModel < reqShaderModel) {
			AX_LOG("DX12 supported shader model {?:X} < {?:X} is required for DXIL shader", static_cast<int>(s.HighestShaderModel), static_cast<int>(reqShaderModel));
//			throw axError_Undefined(AX_LOC);
		}

		if (s.HighestShaderModel >= D3D_SHADER_MODEL_5_1) {
			_adapterInfo.computeShader = true;
		}
	}

	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS  s = {};
		hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &s, sizeof(s));
		Util::throwIfError(hr);
		_adapterInfo.shaderFloat64 = s.DoublePrecisionFloatShaderOps;
	}

#if AX_USE_DX12_DXR
	auto rt = axSPtr_new(axDX12RayTracing());
	if (rt->create()) {
		_rayTracing = rt;
	}
#endif

}

void axDX12Renderer::onGetMemoryInfo(MemoryInfo& info) {
	DXGI_QUERY_VIDEO_MEMORY_INFO  i;
	auto hr = _dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &i);
	Util::throwIfError(hr);

	info.used = i.CurrentUsage;
	info.budget = i.Budget;
}

void axDX12Renderer::_getHardwareAdpter() {
	for (UINT i = 0; ; ++i) {
		axComPtr<IDXGIAdapter1> adapter;
		auto hr =  _dxgiFactory->EnumAdapters1(i, adapter.ptrForInit());
		if (DXGI_ERROR_NOT_FOUND == hr)
			break;

		hr = adapter->QueryInterface(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		Util::throwIfError(hr);

		DXGI_ADAPTER_DESC1 desc;
		_dxgiAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(axDX12_ID3D12Device), nullptr);
		if (!Util::checkError(hr))
			continue;

		_adapterInfo.adapterName.setUtf(axStrView_c_str(desc.Description));
		_adapterInfo.memorySize = desc.DedicatedVideoMemory;

		SIZE_T megaByte = 1024 * 1024;

		AX_LOG(	"DX12 Adpter = {?}\n"
				"	Revision = {?}\n"
				"	VendorId = {?}\n"
				"	DeviceId = {?}\n"
				"	 Video  Memory = {?}M\n"
				"	 System Memory = {?}M\n"
				"	 Shared Memory = {?}M\n",
					desc.Description, 
					desc.Revision,
					desc.VendorId,
					desc.DeviceId,
					desc.DedicatedVideoMemory  / megaByte,
					desc.DedicatedSystemMemory / megaByte,
					desc.SharedSystemMemory    / megaByte);

		return;
	}

	throw axError_Undefined(AX_LOC);
}

#endif //AX_USE_DX12