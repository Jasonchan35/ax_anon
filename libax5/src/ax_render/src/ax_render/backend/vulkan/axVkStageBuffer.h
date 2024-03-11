#pragma once

#if AX_USE_VULKAN

#include "axVkGpuBuffer.h"
#include "../../buffer/axRenderStageBuffer.h"

class axVkStageBuffer : public axRenderStageBuffer {
	AX_RTTI_CLASS(axVkStageBuffer, axRenderStageBuffer);
public:
	using Util = axVkUtil;

	axVkStageBuffer(CreateDesc& desc) : Base(desc) {}

	virtual void* onMap() override {
		auto* p = gpuBuffer();
		return p->_vkDevMem.map(Util::castVkDeviceSize(p->bufferSize()));
	}

	virtual void onUnmap() override {
		auto* p = gpuBuffer();
		p->_vkDevMem.unmap();
	}

	axVkGpuBuffer* gpuBuffer() { return ax_type_cast_debug<axVkGpuBuffer>(_gpuBuffer.ptr()); }

	VkBuffer	vkBuf() { return gpuBuffer()->vkBuf(); }
};

#endif
