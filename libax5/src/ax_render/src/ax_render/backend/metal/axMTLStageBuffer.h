#pragma once

#if AX_USE_METAL

#include "axMTLGpuBuffer.h"
#include "../../buffer/axRenderStageBuffer.h"

class axMTLStageBuffer : public axRenderStageBuffer {
	AX_RTTI_CLASS(axMTLStageBuffer, axRenderStageBuffer);
public:
	using Util = axMTLUtil;

	axMTLStageBuffer(CreateDesc& desc);

	axMTLGpuBuffer*	gpuBuffer() { return ax_type_cast_debug<axMTLGpuBuffer>(_gpuBuffer.ptr()); }

	virtual void* onMap() override {
		auto mtlBuf = gpuBuffer()->mtlBuffer();
		if (!mtlBuf) return nullptr;

		void* p = [mtlBuf contents];
		return p;
	}

	virtual void onUnmap() override {
	}

};

#endif
