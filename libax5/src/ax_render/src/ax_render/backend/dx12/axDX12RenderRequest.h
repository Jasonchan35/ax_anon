#pragma once

#include "../../axRenderRequest.h"
#include "axDX12Texture.h"

class axDX12RenderRequest : public axRenderRequest {
	AX_RTTI_CLASS(axDX12RenderRequest, axRenderRequest)
public:
	using Util = axDX12Util;

	axDX12RenderRequest() {
		auto* dev = Util::d3dDevice();
		{
			auto hr = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ptrForInit()));
			Util::throwIfError(hr);

			hr = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(_cmdList.ptrForInit()));
			Util::throwIfError(hr);

			hr = _cmdList->Close();
			Util::throwIfError(hr);
		}

		{ // compute
			auto hr = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(_computeCmdAllocator.ptrForInit()));
			Util::throwIfError(hr);

			hr = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _computeCmdAllocator, nullptr, IID_PPV_ARGS(_computeCmdList.ptrForInit()));
			Util::throwIfError(hr);

			hr = _computeCmdList->Close();
			Util::throwIfError(hr);
		}

		{
			auto hr = Util::d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_computeFence.ptrForInit()));
			Util::throwIfError(hr);
		}
	}

	virtual void onGpuEnded() override {
		_textureUploadRequests2d.clear();
	}

	axComPtr<ID3D12CommandAllocator>	_cmdAllocator;
	axComPtr<ID3D12CommandAllocator>	_computeCmdAllocator;

	axComPtr<ID3D12GraphicsCommandList> _cmdList;
	axComPtr<ID3D12GraphicsCommandList> _computeCmdList;

	axComPtr<ID3D12Fence>	_computeFence;

	axDX12TextureManager::GpuUploadRequests2D	_textureUploadRequests2d;
};
