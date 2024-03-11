#include "axRenderIndexArray.h"
#include "../axRenderRequest.h"
#include "../axRenderContext.h"

void axRenderIndexArray::create(IndexType indexType, axInt n) {
	clear();
	_indexType = indexType;
	resize(n);
}

void axRenderIndexArray::resize(axInt n) {
	auto old = size();
	_indexData.resize(sizeofIndex() * n);

	if (n > old) {
		setGpuBufferDirty(old, n - old);
	}
}

void axRenderIndexArray::reserve(axInt n) {
	_indexData.reserve(sizeofIndex() * n);
}

void axRenderIndexArray::clear() {
	_indexData.clear();
	_gpuBufferDirtyInBytes.set(0,0);
}

void axRenderIndexArray::copy(const This& r) {
	_indexType = r._indexType;
	_indexData = r._indexData;
}

axRenderGpuBuffer* axRenderIndexArray::uploadToGpu(axRenderRequest& req) {
	AX_ZoneScoped;

	axRenderGpuBuffer* outBuf = nullptr;

	if (!_multiGpuBuffer) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Index;
		_multiGpuBuffer = req.renderer()->createMultiGpuRenderBuffer(desc);
		_gpuBufferDirtyInBytes.set(0, _indexData.size());
	}

	if (_gpuBufferDirtyInBytes.size) {
		outBuf = _multiGpuBuffer->syncData(req, _indexData, _gpuBufferDirtyInBytes);
		_gpuBufferDirtyInBytes.set(0,0);
	} else {
		outBuf = _multiGpuBuffer->currentGpuBuffer();
	}
	return outBuf;
}
