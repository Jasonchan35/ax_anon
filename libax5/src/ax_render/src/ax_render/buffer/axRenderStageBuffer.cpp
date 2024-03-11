#include "axRenderStageBuffer.h"
#include "../axRenderer.h"

axRenderStageBuffer::axRenderStageBuffer(CreateDesc& desc) {
	axRenderGpuBuffer_CreateDesc gpuBufDesc;
	gpuBufDesc.type = axRenderBufferType::Stage;
	gpuBufDesc.byteStride = desc.byteStride;

	_gpuBuffer = axRenderer::s_instance()->createGpuBuffer(gpuBufDesc);
	_gpuBuffer->create(gpuBufDesc, desc.bufferSize);
}
