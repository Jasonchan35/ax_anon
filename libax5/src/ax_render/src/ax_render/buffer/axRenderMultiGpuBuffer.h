#pragma once

#include "axRenderGpuBuffer.h"

class axRenderMultiGpuBuffer : public axRenderResource {
public:
	using Type = axRenderBufferType;
	using CreateDesc = axRenderGpuBuffer_CreateDesc;
	using GpuBuffer = axRenderGpuBuffer;

	struct Buffer {
		axSPtr<axRenderGpuBuffer>	gpuBuffer;
		axIntRange					updatedRange;
	};

	void create(const CreateDesc& desc);

	axRenderGpuBuffer* currentGpuBuffer() { auto* p = currentBuffer(); return p ? p->gpuBuffer.ptr() : nullptr; }

	axRenderGpuBuffer* syncData(axRenderRequest& req, axByteSpan data) { return syncData(req, data, axIntRange(0, data.size())); }
	axRenderGpuBuffer* syncData(axRenderRequest& req, axByteSpan data, axIntRange updatedRange);

friend class axRenderer;
protected:
	axRenderMultiGpuBuffer() = default;

private:

	Buffer*	currentBuffer() { return _buffers.inBound(_bufferIndex) ? &_buffers[_bufferIndex] : nullptr; }
	Buffer* nextBuffer(axRenderRequest& req, axInt bufferSize);

	static const axInt _maxBufferCount = axRender_kMaxFrameAheadCount;
	axArray<Buffer, axRender_kMaxFrameAheadCountHardLimit>	_buffers;
	axInt _bufferIndex = 0;
	axRenderSeqId _lastUpdateRenderSeqId = 0;
	CreateDesc _desc;
};
