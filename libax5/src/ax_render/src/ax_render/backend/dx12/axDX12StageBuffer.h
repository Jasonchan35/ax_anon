#pragma once

#if AX_USE_DX12

#include "axDX12GpuBuffer.h"

class axDX12StageBuffer : public axRenderStageBuffer {
	AX_RTTI_CLASS(axDX12StageBuffer, axRenderStageBuffer);
public:
	using Util = axDX12Util;

	axDX12StageBuffer(CreateDesc& desc) : Base(desc) {}

	axDX12GpuBuffer* gpuBuffer() { return ax_type_cast_debug<axDX12GpuBuffer>(_gpuBuffer.ptr()); }

	ID3D12Resource*	d3dResource() { return gpuBuffer()->d3dResource(); }

	void* onMap() override {
		auto* buf = gpuBuffer();

		D3D12_RANGE readRange = {0, Util::castUINT64(buf->bufferSize()) }; // We do not intend to read from this resource on the CPU.
		UINT8* dst = nullptr;

		auto hr = buf->d3dResource()->Map(0, &readRange, reinterpret_cast<void**>(&dst));
		Util::throwIfError(hr);

		return dst;
	}

	void onUnmap() override {
		auto* buf = gpuBuffer();
		if (!buf) throw axError_Undefined(AX_LOC);
		buf->d3dResource()->Unmap(0, nullptr);
	}
};

#endif