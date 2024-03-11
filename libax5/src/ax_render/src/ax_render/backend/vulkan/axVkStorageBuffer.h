#pragma once

#if AX_USE_VULKAN

#include "axVkGpuBuffer.h"
#include "../../buffer/axRenderStorageBuffer.h"

class axVkStorageBuffer : public axRenderStorageBuffer {
	AX_RTTI_CLASS(axVkStorageBuffer, axRenderStorageBuffer);
public:
	using Util = axVkUtil;

	axVkStorageBuffer(CreateDesc& desc);

	VkBuffer	vkBuf() { return ax_type_cast_debug<axVkGpuBuffer>(_gpuBuffer.ptr())->vkBuf(); }
};

#endif