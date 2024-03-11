#if AX_USE_VULKAN

#include "axCppVk/axCppVk.h"
#include "axVkCommandDispatcher.h"
#include "axVkMaterial.h"
#include "axVkRenderContext_Base.h"
#include "axVkTexture.h"
#include "axVkStorageBuffer.h"
#include "axVkStageBuffer.h"
#include "axVkFence.h"

axVkCommandDispatcher::axVkCommandDispatcher(axVkRenderContext_Base* renderContext_, axVkRenderRequest* renderRequest_)
	: Base(renderContext_, renderRequest_)
	, renderContext(renderContext_)
	, req(renderRequest_)
{
}

void axVkCommandDispatcher::onUploadTexture2D(axTexture2D_UploadRequest& uploadReq) {
	auto* tex = ax_type_cast<axVkTexture2D>(uploadReq.tex.ptr());
	if (!tex) {
		throw axError_Undefined(AX_LOC);
	}
	tex->onUploadRequest(*this, uploadReq);
}

void axVkCommandDispatcher::onGpuFrameBegin() {
	if (!cmdBuffer || !cmdBuffer->handle()) {
		throw axError_Undefined(AX_LOC);
	}

	Base::onGpuFrameBegin();
	auto* texMgr = axVkTextureManager::s_instance();
	texMgr->onGpuFrameBegin(*this);
}

void axVkCommandDispatcher::onGpuFrameEnd() {
	auto* texMgr = axVkTextureManager::s_instance();
	texMgr->onGpuFrameEnd(*this);

	Base::onGpuFrameEnd();
}

void axVkCommandDispatcher::cmdSwapBuffers(axRenderCommand_SwapBuffers& cmd) {
	renderContext->cmdSwapBuffers(*this);
}

void axVkCommandDispatcher::cmdClearColorAndDepthBuffers(axRenderCommand_ClearColorAndDepthBuffers& cmd) {
	VkExtent2D size = frameBuffer->size();

	axFixedArray<VkClearValue, 2> attachmentClearValues;
	auto& col = attachmentClearValues[0].color;
	col.float32[0] = cmd.color.r;
	col.float32[1] = cmd.color.g;
	col.float32[2] = cmd.color.b;
	col.float32[3] = cmd.color.a;

	auto& ds = attachmentClearValues[1].depthStencil;
	ds.depth = cmd.depth;
	ds.stencil = 0;
	
	cmdBuffer->beginRenderPass(renderPass, *frameBuffer, size, attachmentClearValues);
}

void axVkCommandDispatcher::cmdSetViewport(axRenderCommand_SetViewport& cmd) {
	auto rc = renderContext->nativeViewRect();
	auto srcVp = cmd.viewport.flipYWithinContainer(rc.h);

	VkViewport	vp;
	vp.x		= srcVp.x;
	vp.y		= rc.h - srcVp.y;
	vp.width	= srcVp.w;
	vp.height	= -srcVp.h;
	vp.minDepth	= 0;
	vp.maxDepth	= 1;

	vkCmdSetViewport(*cmdBuffer, 0, 1, &vp);
}

void axVkCommandDispatcher::cmdComputeCall(axRenderCommand_ComputeCall& cmd) {
	auto* material = ax_type_cast<axVkMaterial>(cmd.computeCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(*this, cmd.computeCall);
	cmdBuffer->setCheckPoint("Compute - Dispatch");

	auto tc = axVec3<uint32_t>::s_cast(cmd.computeCall.threadGroupCount);
	vkCmdDispatch(*cmdBuffer, tc.x, tc.y, tc.z);
}

void axVkCommandDispatcher::cmdCopyToStageBuffer(axRenderCommand_CopyToStageBuffer& cmd) {
	auto* dst = ax_type_cast<axVkStageBuffer	>(cmd.dst.ptr());
	auto* src = ax_type_cast<axVkStorageBuffer	>(cmd.src.ptr());

	if (!dst || !src) throw axError_Undefined(AX_LOC);

	auto* dstVk = dst->vkBuf();
	auto* srcVk = src->vkBuf(); 

	if (!dstVk || !srcVk) throw axError_Undefined(AX_LOC);

	VkBufferCopy region = {};
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size = Util::castVkDeviceSize(src->bufferSize());

	vkCmdCopyBuffer(*cmdBuffer, srcVk, dstVk, 1, &region);
}

void axVkCommandDispatcher::cmdDrawCall(axRenderCommand_DrawCall& cmd) {
	auto& drawCall = cmd.drawCall;
	auto* material = ax_type_cast<axVkMaterial>(drawCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(*this, drawCall);

	{ // scissor rect
		auto rc = drawCall.scissorWorldRect - renderContext->worldPos();
		VkRect2D	scissor;
		scissor.offset.x		= static_cast<int32_t >(rc.x);
		scissor.offset.y		= static_cast<int32_t >(rc.y);
		scissor.extent.width	= static_cast<uint32_t>(rc.w);
		scissor.extent.height	= static_cast<uint32_t>(rc.h);
		cmdBuffer->setCheckPoint("set scissor");
		vkCmdSetScissor(*cmdBuffer, 0, 1, &scissor);
	}

	{ // bind vertex buffer
		if (!drawCall.vertexBuffer) throw axError_Undefined(AX_LOC);
		auto* vb = ax_type_cast<axVkGpuBuffer>(drawCall.vertexBuffer.ptr());
		if (!vb) throw axError_Undefined(AX_LOC);

		VkBuffer buf = vb->vkBuf();
		VkDeviceSize vertexOffset = Util::castUInt32(drawCall.vertexByteOffset);
		cmdBuffer->setCheckPoint("bind vertex");
		vb->resourceToKeepInFrame(*this);
		vkCmdBindVertexBuffers(*cmdBuffer, ax_enum_int(axVkBufferIndexUsage::VertexAttr), 1, &buf, &vertexOffset);
	}

	if (!drawCall.isIndexEnabled()) {
		cmdBuffer->setCheckPoint("draw");
		vkCmdDraw(*cmdBuffer, Util::castUInt32(drawCall.vertexCount), 1, 0, 0);
	} else {
		auto indexType = Util::getVkIndexType(drawCall.indexType);
		if (!drawCall.indexBuffer) throw axError_Undefined(AX_LOC);
		auto* ib = ax_type_cast<axVkGpuBuffer>(drawCall.indexBuffer.ptr());
		if (!ib) throw axError_Undefined(AX_LOC);

		auto indexOffset = Util::castVkDeviceSize(drawCall.indexByteOffset);
		cmdBuffer->setCheckPoint("bind index");
		vkCmdBindIndexBuffer(*cmdBuffer, ib->vkBuf(), indexOffset, indexType);

		cmdBuffer->setCheckPoint("draw Indexed");
		ib->resourceToKeepInFrame(*this);
		vkCmdDrawIndexed(*cmdBuffer, Util::castUInt32(drawCall.indexCount), 1, 0, 0, 0);
	}
}

#endif
