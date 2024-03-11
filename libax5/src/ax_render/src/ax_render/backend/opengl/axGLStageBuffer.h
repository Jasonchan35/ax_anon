#pragma once

#if AX_USE_OPENGL

#include "axGLGpuBuffer.h"

class axGLStageBuffer : public axRenderStageBuffer {
	AX_RTTI_CLASS(axGLStageBuffer, axRenderStageBuffer);
public:
	using Util = axGLUtil;

	axGLStageBuffer(CreateDesc& desc) : Base(desc) {}

	axGLGpuBuffer*	gpuBuffer() { return ax_type_cast_debug<axGLGpuBuffer>(_gpuBuffer.ptr()); }

	virtual void* onMap() override {
		auto* buf = gpuBuffer();
		if (!buf) return nullptr;

//		auto* p = glMapNamedBufferRange(buf->handle(), 0, buf->bufferSize(), GL_MAP_READ_BIT);
		auto* p = buf->map(0, buf->bufferSize(), GL_MAP_READ_BIT);
		Util::checkGLError(AX_LOC);
		return p;
	}
	virtual void onUnmap() override {
		auto* buf = gpuBuffer();
		if (!buf) return;
		buf->unmap();
//		glUnmapNamedBuffer(buf->handle());
	}
};

#endif
