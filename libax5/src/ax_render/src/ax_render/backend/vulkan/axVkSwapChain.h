#pragma once

#if AX_USE_VULKAN

#include "axVkCommonBase.h"
#include "axCppVk/axCppVk.h"

class axVkRenderer;
class axVkRenderContext;
class axVkRenderContext_Base;

class axVkSwapChain : public axObject {
public:
	using Util = axVkUtil;

	axVkSwapChain(axVkRenderContext_Base& ctx, axVkSwapChain* oldSwapChain);
	~axVkSwapChain();

	class GpuFrame : public axNonCopyable {
	public:
		VkImage						_colorImage = nullptr;
		axCppVkImageView			_colorImageView;
		axCppVkFramebuffer			_frameBuffer;
	};

	axCppVkSwapchainKHR			_swapChain;
	axCppVkImage				_depthImage;
	axCppVkDeviceMemory			_depthMemory;
	axCppVkImageView			_depthImageView;

	axArray<GpuFrame, 4>		_gpuFrames;
	axInt						_gpuFrameIndex = 0;

	GpuFrame& currentGpuFrame() {
		return _gpuFrames[_gpuFrameIndex];
	}	
};
#endif
