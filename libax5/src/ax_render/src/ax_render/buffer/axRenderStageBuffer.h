#pragma once

#include "../axRenderResource.h"
#include "axRenderGpuBuffer.h"

class axRenderStageBuffer_CreateDesc : public axRenderGpuBuffer_CreateDesc {
public:
	axInt	bufferSize = 0;
	axInt	byteStride = 0;
};

//! Access for both CPU <-> GPU
class axRenderStageBuffer : public axRenderResource {
public:
	using CreateDesc = axRenderStageBuffer_CreateDesc;

	axInt bufferSize() { return _gpuBuffer ? _gpuBuffer->bufferSize() : 0; }

	struct ScopedMap : public axNonCopyable {
		ScopedMap(axRenderStageBuffer* buf)
			: _buf(buf)
		{
			if (buf) {
				void* p = _buf->onMap();
				if (!p) throw axError_Undefined(AX_LOC);
				_span = axByteMutSpan(reinterpret_cast<axByte*>(p), buf->bufferSize());
			}
		}

		ScopedMap(ScopedMap && r) {
			_buf = r._buf;
			r._buf = nullptr;
		}

		~ScopedMap() {
			if (_buf) {
				_buf->onUnmap();
			}
		}

		axByteMutSpan	span() & { return _span; }

	private:
		axByteMutSpan	_span;
		axRenderStageBuffer* _buf = nullptr;
	};

	ScopedMap	map() { return ScopedMap(this); }

protected:
	axRenderStageBuffer(CreateDesc& desc);

	virtual void* onMap() = 0;
	virtual void onUnmap() = 0;

	axSPtr<GpuBuffer>	_gpuBuffer;

	bool _readyToRead = false;
};
