#pragma once

#if AX_USE_DX12

#include "axDX12GpuBuffer.h"
#include <ax_render/buffer/axRenderStorageBuffer.h>

class axDX12StorageBuffer : public axRenderStorageBuffer {
	AX_RTTI_CLASS(axDX12StorageBuffer, axRenderStorageBuffer);
public:
	using Util = axDX12Util;

	axDX12StorageBuffer(CreateDesc& desc) : Base(desc) {}

	axDX12GpuBuffer* gpuBuffer() { return ax_type_cast_debug<axDX12GpuBuffer>(_gpuBuffer.ptr()); }
	ID3D12Resource*	d3dResource() { return gpuBuffer()->d3dResource(); }
};

#endif