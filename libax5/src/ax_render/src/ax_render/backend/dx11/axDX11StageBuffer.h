#pragma once

#if AX_USE_DX11

#include "axDX11GpuBuffer.h"
#include "../../buffer/axRenderStageBuffer.h"

class axDX11StageBuffer : public axRenderStageBuffer {
	AX_RTTI_CLASS(axDX11StageBuffer, axRenderStageBuffer);
public:
	using Util = axDX11Util;

	axDX11StageBuffer(CreateDesc& desc);

	virtual void* onMap() override {
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		auto* ctx = Util::d3dContext();
		auto hr = ctx->Map(gpuBuffer()->_d3dBuf, 0, D3D11_MAP_READ_WRITE, 0, &mapped);
		Util::throwIfError(hr);
		return mapped.pData;
	}

	virtual void onUnmap() override {
		auto* ctx = Util::d3dContext();
		ctx->Unmap(gpuBuffer()->_d3dBuf, 0);
	}

	axDX11GpuBuffer*	gpuBuffer() { return ax_type_cast_debug<axDX11GpuBuffer>(_gpuBuffer.ptr()); }
};

#endif
