#pragma once

#if AX_USE_DX11

#include "axDX11GpuBuffer.h"
#include "../../buffer/axRenderStorageBuffer.h"

//! storage only on GPU side
class axDX11StorageBuffer : public axRenderStorageBuffer {
	AX_RTTI_CLASS(axDX11StorageBuffer, axRenderStorageBuffer);
public:
	using Util = axDX11Util;

	axDX11StorageBuffer(CreateDesc& desc);

	axDX11GpuBuffer* gpuBuffer() { return ax_type_cast<axDX11GpuBuffer>(_gpuBuffer.ptr()); }


	axComPtr<ID3D11UnorderedAccessView> _uav;
};

#endif