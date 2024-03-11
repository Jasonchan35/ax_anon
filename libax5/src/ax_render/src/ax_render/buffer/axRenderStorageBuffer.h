#pragma once

#include "../axRenderResource.h"
#include "axRenderGpuBuffer.h"

class axRenderStorageBuffer_CreateDesc {
public:
	axInt	bufferSize = 0;
	axInt	byteStride = 16;
};

class axRenderStorageBuffer : public axRenderResource {
public:
	using CreateDesc = axRenderStorageBuffer_CreateDesc;

	axInt bufferSize() const { return _gpuBuffer ? _gpuBuffer->bufferSize() : 0; }

	GpuBuffer* gpuBuffer() { return _gpuBuffer; }

protected:
	axRenderStorageBuffer(CreateDesc& desc);

	axSPtr<GpuBuffer>	_gpuBuffer;
};
