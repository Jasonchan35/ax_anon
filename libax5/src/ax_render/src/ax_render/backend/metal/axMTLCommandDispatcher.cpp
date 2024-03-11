#if AX_USE_METAL

#include "axMTLCommandDispatcher.h"
#include "axMTLRenderContext.h"
#include "axMTLMaterial.h"
#include "axMTLGpuBuffer.h"
#include "axMTLTexture.h"

axMTLCommandDispatcher::axMTLCommandDispatcher(axMTLRenderContext_Base* renderContext_, axRenderRequest* renderReq)
	: Base(renderContext_, renderReq)
	, renderContext(renderContext_)
{
	cmdEncoder = renderContext->_cmdEncoder;
	computeCmdEncoder = renderContext->_computeCmdEncoder;
	blitCmdEncoder = renderContext->_blitCmdEncoder;

	mtkView = renderContext->mtkView();
}

void axMTLCommandDispatcher::onUploadTexture2D(axTexture2D_UploadRequest& req) {
	auto* tex = ax_type_cast<axMTLTexture2D>(req.tex.ptr());
	if (!tex) {
		throw axError_Undefined(AX_LOC);
	}
	tex->onUploadRequest(*this, req);
}

void axMTLCommandDispatcher::onGpuFrameBegin() {
	Base::onGpuFrameBegin();
}

void axMTLCommandDispatcher::onGpuFrameEnd() {
	Base::onGpuFrameEnd();
}

void axMTLCommandDispatcher::cmdSwapBuffers(axRenderCommand_SwapBuffers& cmd) {
	[computeCmdEncoder endEncoding];
	[renderContext->_computeCmdBuffer commit];

	[cmdEncoder endEncoding];

	id<CAMetalDrawable> drawable = [mtkView currentDrawable];
	if (drawable) {
		[renderContext->_cmdBuffer presentDrawable:drawable];
	}

	[blitCmdEncoder endEncoding];
	[renderContext->_blitCmdBuffer commit];
}

void axMTLCommandDispatcher::cmdClearColorAndDepthBuffers(axRenderCommand_ClearColorAndDepthBuffers& cmd) {
	// TODO:
}

void axMTLCommandDispatcher::cmdSetViewport(axRenderCommand_SetViewport& cmd) {
	MTLViewport mtkViewport;
	mtkViewport.originX	= cmd.viewport.x;
	mtkViewport.originY	= cmd.viewport.y;
	mtkViewport.width	= cmd.viewport.w;
	mtkViewport.height	= cmd.viewport.h;
	mtkViewport.znear	= 0.0;
	mtkViewport.zfar	= 1.0;
	[cmdEncoder setViewport:mtkViewport];
}

void axMTLCommandDispatcher::cmdComputeCall(axRenderCommand_ComputeCall& cmd) {
	auto& computeCall = cmd.computeCall;
	auto* material = ax_type_cast<axMTLMaterial>(computeCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(*this, computeCall);

	auto tc_ = axVec3<NSUInteger>::s_cast(computeCall.threadGroupCount);
	auto tc = MTLSizeMake(tc_.x, tc_.y, tc_.z);

	auto* shaderPass = ax_type_cast_debug<axMTLShaderPass>(material->shader()->findPass(computeCall.renderPass));
	if (!shaderPass) throw axError_Undefined(AX_LOC);
	auto wg = axVec3<NSUInteger>::s_cast(shaderPass->_csWorkgroupSize);
	auto threadPerGroup = MTLSizeMake(wg.x, wg.y, wg.z);

	[computeCmdEncoder dispatchThreadgroups:tc threadsPerThreadgroup:threadPerGroup];
}

void axMTLCommandDispatcher::cmdCopyToStageBuffer(axRenderCommand_CopyToStageBuffer& cmd) {
	auto* dst = ax_type_cast_debug<axMTLStageBuffer  >(cmd.dst.ptr())->gpuBuffer();
	auto* src = ax_type_cast_debug<axMTLStorageBuffer>(cmd.src.ptr())->gpuBuffer();

	if (!dst || !src) throw axError_Undefined(AX_LOC);

	auto* dstMtl = dst->mtlBuffer();
	auto* srcMtl = src->mtlBuffer();

	if (!dstMtl || !srcMtl) throw axError_Undefined(AX_LOC);

	auto dataSize = Util::castNSUInt(src->bufferSize());

	[blitCmdEncoder copyFromBuffer:srcMtl sourceOffset:0
					toBuffer:dstMtl  destinationOffset:0
					size:dataSize];
}

void axMTLCommandDispatcher::cmdDrawCall(axRenderCommand_DrawCall& cmd) {
	auto& drawCall = cmd.drawCall;
	auto* material = ax_type_cast<axMTLMaterial>(drawCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(*this, drawCall);

	{
		Rect2 rc = drawCall.scissorWorldRect - renderContext->worldPos();

		// metal does not allow scissor rect > drawable size
		auto drawableSize = Vec2(mtkView.drawableSize);
		rc = rc.intersects(Rect2(Vec2(0,0), drawableSize));

		MTLScissorRect scissorRect;
		scissorRect.x		= static_cast<NSUInteger>(rc.x);
		scissorRect.y		= static_cast<NSUInteger>(rc.y);
		scissorRect.width	= static_cast<NSUInteger>(rc.w);
		scissorRect.height	= static_cast<NSUInteger>(rc.h);
		[cmdEncoder setScissorRect:scissorRect];
	}

	if (drawCall.primitiveType == axRenderPrimitiveType::Unknown)
		throw axError_Undefined(AX_LOC);

	auto pmt = Util::getMTLPrimitiveType(drawCall.primitiveType);

	auto* vertexBuffer = ax_type_cast<axMTLGpuBuffer>(drawCall.vertexBuffer.ptr());
	if (!vertexBuffer) throw axError_Undefined(AX_LOC);

	auto mtlVertexBuffer = vertexBuffer->mtlBuffer();
	if (!mtlVertexBuffer) throw axError_Undefined(AX_LOC);

	auto vertexByteOffset = Util::castNSUInt(drawCall.vertexByteOffset);
	[cmdEncoder setVertexBuffer:mtlVertexBuffer offset:vertexByteOffset atIndex: ax_enum_int(Util::BufferIndexUsage::VertexAttr)];

	if (!drawCall.isIndexEnabled()) {
		[cmdEncoder drawPrimitives:	pmt
						vertexStart:	0
						vertexCount:	Util::castNSUInt(drawCall.vertexCount)
						instanceCount:	1
						baseInstance:	0];
	} else {
		auto* indexBuffer = ax_type_cast<axMTLGpuBuffer>(drawCall.indexBuffer.ptr());
		if (!indexBuffer) throw axError_Undefined(AX_LOC);

		auto mtlIndexType = Util::getMTLIndexType(drawCall.indexType);

		auto mtlIndexBuffer = indexBuffer->mtlBuffer();
		if (!mtlIndexBuffer) throw axError_Undefined(AX_LOC);

		if (drawCall.indexByteOffset % 4) {
			AX_LOG("indexBufferOffset ({?}) must be aligned to 4 bytes.", drawCall.indexByteOffset);
		}
		[cmdEncoder	drawIndexedPrimitives: pmt
						indexCount: 		Util::castNSUInt(drawCall.indexCount)
						indexType: 			mtlIndexType
						indexBuffer: 		mtlIndexBuffer
						indexBufferOffset: 	Util::castNSUInt(drawCall.indexByteOffset)
						instanceCount:		1
						baseVertex:			0
						baseInstance:		0];
	}
}

#endif
