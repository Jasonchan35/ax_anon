#pragma once

#if AX_USE_DX12

#include "../../buffer/axRenderGpuBuffer.h"
#include "axDX12Resource.h"

class axDX12GpuBuffer : public axRenderGpuBuffer {
	AX_RTTI_CLASS(axDX12GpuBuffer, axRenderGpuBuffer);
public:
	using Util = axDX12Util;

	void onCreate(const CreateDesc& desc, axInt bufferSize) {
		_p.create(desc.type, bufferSize);
	}

	virtual void onUploadToGpu(axInt offset, axByteSpan data) final {
		_p.uploadToGpu(offset, data);
	}

	axDX12Resource_Buffer& resource() { return _p; }

	ID3D12Resource*	d3dResource() { return _p.d3dResource(); }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _p.gpuAddress(); }


private:
	axDX12Resource_Buffer	_p;
};

#endif