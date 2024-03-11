#include "axRenderMultiGpuBuffer.h"
#include "../axRenderer.h"
#include "../axRenderCommandDispatcher.h"

void axRenderMultiGpuBuffer::create(const CreateDesc& desc)  {
	if (desc.type == Type::Unknown) {
		throw axError_Undefined(AX_LOC);
	}
	_desc = desc;
}

axRenderMultiGpuBuffer::Buffer* axRenderMultiGpuBuffer::nextBuffer(axRenderRequest& req, axInt bufferSize) {
	if (_buffers.size()) {
		_bufferIndex = (_bufferIndex + 1) % _maxBufferCount;
	}

	_buffers.resize(_bufferIndex + 1);
	auto& p = _buffers[_bufferIndex];
	if (!p.gpuBuffer || p.gpuBuffer->bufferSize() < bufferSize) {

		if (p.gpuBuffer) {
			axInt oldSize = p.gpuBuffer->bufferSize();
			ax_max_it(bufferSize, ax_next_pow2_half(oldSize + 1));
		}

		p.gpuBuffer = req.renderer()->createGpuBuffer(_desc);
		p.gpuBuffer->create(_desc, bufferSize);
	}
	return &p;
}

axRenderGpuBuffer* axRenderMultiGpuBuffer::syncData(axRenderRequest& req, axByteSpan data, axIntRange updatedRange) {
	AX_ZoneScoped;

	if (_lastUpdateRenderSeqId == req.renderSeqId()) {
		AX_ASSERT_MSG(false, "update same render buffer twice in single frame ?");
		return currentBuffer()->gpuBuffer;
	}
	_lastUpdateRenderSeqId = req.renderSeqId();

	auto finalUpdatedRange = updatedRange;
	axIntRange wholeDataRange(0, data.size());

	auto* p = currentBuffer();
	if (!p || p->gpuBuffer->bufferSize() < data.sizeInBytes()) {
		p = nextBuffer(req, data.sizeInBytes());
		p->updatedRange = updatedRange;

		finalUpdatedRange = wholeDataRange;
	} else {

		p->updatedRange = updatedRange;
		for (auto& q: _buffers) {
			finalUpdatedRange |= q.updatedRange;
		}
		finalUpdatedRange &= wholeDataRange; // ensure not out of range
	}

	req.statistics.renderBufferUploadBytes += finalUpdatedRange.size;
	p->gpuBuffer->uploadToGpu(finalUpdatedRange.start, data.slice(finalUpdatedRange));
	return p->gpuBuffer;
}
