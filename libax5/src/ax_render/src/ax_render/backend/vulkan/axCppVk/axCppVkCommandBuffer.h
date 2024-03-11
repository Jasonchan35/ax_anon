#pragma once

#if AX_USE_VULKAN

#include "axCppVkCommandPool.h"
#include "axCppVkRenderPass.h"
#include "axCppVkFramebuffer.h"

class axCppVkCommandBuffer : public axCppVkBase {
public:
	const VkCommandBuffer& handle() { return _h; }
	operator const VkCommandBuffer&() { return _h; }

	axCppVkCommandBuffer() = default;
	axCppVkCommandBuffer(axCppVkCommandBuffer && r) {
		_h = r._h;
		_cmdPool = r._cmdPool;

		r._h = VK_NULL_HANDLE;
		r._cmdPool = nullptr;
	}

	~axCppVkCommandBuffer() { destroy(); }

	void destroy();
	void create(axCppVkCommandPool& cmdPool);

	void beginCommand();
	void endCommand();

	void beginRenderPass(	VkRenderPass renderPass, 
							VkFramebuffer frameBuffer,
							const VkExtent2D& extent,
							axSpan<VkClearValue> attachmentClearValues);
	void endRenderPass();

	void setCheckPoint(axStrLiteral s);

private:
	VkCommandBuffer _h = VK_NULL_HANDLE;
	axCppVkCommandPool* _cmdPool = nullptr;
};

#endif