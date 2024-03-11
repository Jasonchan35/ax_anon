#pragma once

#if AX_USE_VULKAN

#include "axCppVk/axCppVkDeviceMemory.h"
#include "../../buffer/axRenderGpuBuffer.h"

class axVkGpuBuffer : public axRenderGpuBuffer {
	AX_RTTI_CLASS(axVkGpuBuffer, axRenderGpuBuffer)
public:
	using Util = axVkUtil;

	virtual void onCreate(const CreateDesc& desc, axInt bufferSize) final;
	virtual void onUploadToGpu(axInt offset, axByteSpan data) final;

	VkBuffer		vkBuf() { return _vkBuf; }

	void resourceToKeepInFrame(axVkCommandDispatcher& dispatcher);

	axInt _lastUsedRenderSeqId = 0;
	axCppVkBuffer		_vkBuf;
	axCppVkDeviceMemory	_vkDevMem;
};

class axVkStagingBuffer {
public:
	axCppVkBuffer		_buf;
	axCppVkDeviceMemory	_devMem;
};


#endif