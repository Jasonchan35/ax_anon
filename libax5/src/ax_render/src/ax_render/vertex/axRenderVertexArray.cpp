#include "axRenderVertexArray.h"
#include "../axRenderRequest.h"
#include "../axRenderContext.h"

void axRenderVertexArray::create(VertexDesc desc, axInt n) {
	clear();
	_desc = desc;
	AX_ASSERT(_desc != nullptr);
	resize(n);
}

void axRenderVertexArray::resize(axInt n) {
	auto sizeInBytes = _desc->strideBytes * n;
	auto old = size();
	_vertexData.resize(sizeInBytes);
	if (n > old) {
		setGpuBufferDirty(old, n - old);
	}
}

void axRenderVertexArray::reserve(axInt n) {
	auto sizeInBytes = _desc->strideBytes * n;
	_vertexData.reserve(sizeInBytes);
}

void axRenderVertexArray::clear() {
	_vertexData.clear();
	_gpuBufferDirtyInBytes.set(0,0);
}

void axRenderVertexArray::setGpuBufferDirty(axInt start, axInt n) {
	_gpuBufferDirtyInBytes |= axIntRange(start, n) * _desc->strideBytes;
}

void axRenderVertexArray::copy(const This& r) {
	_desc		= r._desc;
	_vertexData	= r._vertexData;
	setGpuBufferDirty(0, size());
}

void axRenderVertexArray::append(const This& r) {
	if (_desc->vertexType != r._desc->vertexType) {
		throw axError_Undefined(AX_LOC);
	}

	setGpuBufferDirty(size(), r.size());
	_vertexData.appendRange(r._vertexData);
}

axBBox3f axRenderVertexArray::calcBoundingBox() const {
	axBBox3f box;
	for (auto& pos : positions()) {
		box.encapsulate(pos);
	}
	return box;
}

void axRenderVertexArray::transform(const axMat4f& mat) {
	for (auto& pos : editPositions()) {
		pos = mat.mulPoint(pos);
	}
}

axRenderGpuBuffer* axRenderVertexArray::uploadToGpu(axRenderRequest& req) {
	AX_ZoneScoped;

	axRenderGpuBuffer* outBuf = nullptr;

	if (!_multiGpuBuffer) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Vertex;
		_multiGpuBuffer = req.renderer()->createMultiGpuRenderBuffer(desc);
		_gpuBufferDirtyInBytes.set(0, _vertexData.size());
	}

	if (_gpuBufferDirtyInBytes.size) {
		outBuf = _multiGpuBuffer->syncData(req, _vertexData, _gpuBufferDirtyInBytes);
		_gpuBufferDirtyInBytes.set(0,0);
	} else {
		outBuf = _multiGpuBuffer->currentGpuBuffer();
	}

	return outBuf;
}
