#include "axRenderStorageBuffer.h"

axRenderStorageBuffer::axRenderStorageBuffer(CreateDesc& desc) {
	axRenderGpuBuffer_CreateDesc gpuBufDesc;
	gpuBufDesc.type = axRenderBufferType::Storage;
	gpuBufDesc.byteStride = desc.byteStride;

	_gpuBuffer = axRenderer::s_instance()->createGpuBuffer(gpuBufDesc);
	_gpuBuffer->create(gpuBufDesc, desc.bufferSize);
}
