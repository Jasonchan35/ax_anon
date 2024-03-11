#if AX_USE_VULKAN

#include "axCppVkCommandBuffer.h"

void axCppVkCommandBuffer::destroy() {
	if (_h) {
		vkFreeCommandBuffers(_cmdPool->device(), *_cmdPool, 1, &_h);
		_h = nullptr;
		_cmdPool = nullptr;
	}
}

void axCppVkCommandBuffer::create(axCppVkCommandPool& cmdPool) {
	destroy();
	_cmdPool = &cmdPool;

	VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
	cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocInfo.pNext = nullptr;
	cmdBufAllocInfo.commandPool = cmdPool;
	cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufAllocInfo.commandBufferCount = 1;

	auto err = vkAllocateCommandBuffers(cmdPool.device(), &cmdBufAllocInfo, &_h);
	Util::throwIfError(err);
}

void axCppVkCommandBuffer::beginCommand() {
	setCheckPoint("beginCommand");

	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	info.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(_h, &info);
}

void axCppVkCommandBuffer::endCommand() {
	setCheckPoint("endCommand");

	vkEndCommandBuffer(_h);
}

void axCppVkCommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer frameBuffer, const VkExtent2D& size, axSpan<VkClearValue> attachmentClearValues) {
	setCheckPoint("beginRenderPass");

	VkRenderPassBeginInfo info = {};
	info.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext					= nullptr;
	info.renderPass				= renderPass;
	info.framebuffer			= frameBuffer;
	info.renderArea.offset.x	= 0;
	info.renderArea.offset.y	= 0;
	info.renderArea.extent		= size;
	info.clearValueCount		= Util::castUInt32(attachmentClearValues.size());
	info.pClearValues			= attachmentClearValues.data();

	vkCmdBeginRenderPass(_h, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void axCppVkCommandBuffer::endRenderPass() {
	setCheckPoint("endRenderPass");
	vkCmdEndRenderPass(_h);
}

void axCppVkCommandBuffer::setCheckPoint(axStrLiteral s) {
	// Util::renderer()->procList()->vkCmdSetCheckpointNV(_h, s.c_str());
}

#endif
