#pragma once

#if AX_USE_OPENGL

#include "axGLGpuBuffer.h"

class axGLStorageBuffer : public axRenderStorageBuffer {
	AX_RTTI_CLASS(axGLStorageBuffer, axRenderStorageBuffer);
public:
	using Util = axGLUtil;

	axGLStorageBuffer(CreateDesc& desc) : Base(desc) {}

	axGLGpuBuffer* gpuBuffer() { return ax_type_cast<axGLGpuBuffer>(_gpuBuffer.ptr()); }

};

#endif