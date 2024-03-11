#if AX_USE_OPENGL

#include "axGLCommandDispatcher.h"
#include "axGLRenderContext.h"
#include "axGLTexture.h"
#include "axGLMaterial.h"
#include "axGLGpuBuffer.h"
#include "axGLFence.h"
#include "axGLStageBuffer.h"
#include "axGLStorageBuffer.h"

axGLCommandDispatcher::axGLCommandDispatcher(axGLRenderContext_Base* renderContext_, axRenderRequest* renderRequest_) 
	: Base(renderContext_, renderRequest_)
	, renderContext(renderContext_)
	, renderRequest(renderRequest_)
{
}

void axGLCommandDispatcher::onUploadTexture2D(axTexture2D_UploadRequest& req) {
	auto* tex = ax_type_cast<axGLTexture2D>(req.tex.ptr());
	if (!tex) {
		throw axError_Undefined(AX_LOC);
	}
	tex->onUploadRequest(*this, req);
}

void axGLCommandDispatcher::onGpuFrameBegin() {
	Base::onGpuFrameBegin();
}

void axGLCommandDispatcher::onGpuFrameEnd() {
	Base::onGpuFrameEnd();
}

void axGLCommandDispatcher::cmdSwapBuffers(axRenderCommand_SwapBuffers& cmd) {
	renderContext->cmdSwapBuffers();

	{
		auto* fence = ax_type_cast_debug<axGLFence>(renderRequest->completedFence());
		if (!fence) throw axError_Undefined(AX_LOC);
		fence->addToGpu();
	}
}

void axGLCommandDispatcher::cmdClearColorAndDepthBuffers(axRenderCommand_ClearColorAndDepthBuffers& cmd) {
	auto& color = cmd.color;
	::glClearColor(color.r, color.g, color.b, color.a);
	::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	::glClearDepthf(cmd.depth);
	::glDepthMask(GL_TRUE);

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void axGLCommandDispatcher::cmdSetViewport(axRenderCommand_SetViewport& cmd) {
	auto rect = renderContext->nativeViewRect();

	auto flipVp = cmd.viewport.flipYWithinContainer(rect.h);
	auto vp = axRect2<GLint>::s_cast(flipVp);

	::glViewport(vp.x, vp.y, vp.w, vp.h);
}

void axGLCommandDispatcher::cmdCopyToStageBuffer(axRenderCommand_CopyToStageBuffer& cmd) {
	auto* dst = ax_type_cast_debug<axGLStageBuffer	>(cmd.dst.ptr())->gpuBuffer();
	auto* src = ax_type_cast_debug<axGLStorageBuffer>(cmd.src.ptr())->gpuBuffer();

	if (!dst || !src) throw axError_Undefined(AX_LOC);

	glCopyNamedBufferSubData(src->handle(), dst->handle(), 0, 0, src->bufferSize());
	Util::checkGLError(AX_LOC);
}

void axGLCommandDispatcher::cmdComputeCall(axRenderCommand_ComputeCall& cmd) {
	auto* material = ax_type_cast<axGLMaterial>(cmd.computeCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);
	material->bind(*this, cmd.computeCall);

	auto tc = axVec3<GLuint>::s_cast(cmd.computeCall.threadGroupCount);
	glDispatchCompute(tc.x, tc.y, tc.z);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void axGLCommandDispatcher::cmdDrawCall(axRenderCommand_DrawCall& cmd) {
	auto& drawCall = cmd.drawCall;
	//auto* vb = ax_type_cast<axGLRenderVertexBuffer>(_inlineDrawVertexBuffer.ptr());
	//auto* ib = ax_type_cast<axGLRenderIndexBuffer>(_inlineDrawIndexBuffer.ptr());

	auto* mtl = ax_const_cast( ax_type_cast<axGLMaterial>(drawCall.material.ptr()) );
	if (!mtl) return;

	Util::checkGLError(AX_LOC);

	BindShaderPass sp(*this, drawCall, mtl);
	Util::checkGLError(AX_LOC);

	{
		axRect2f rc = drawCall.scissorWorldRect - renderContext->worldPos();
		rc.y = renderContext->nativeViewRect().h - rc.yMax(); // OpenGL using "bottom left" coordinate

		glScissor(	static_cast<GLint  >(rc.x),
					static_cast<GLint  >(rc.y),
					static_cast<GLsizei>(rc.w),
					static_cast<GLsizei>(rc.h));
	}

	GLenum pmt =Util::getGLPrimitiveTopology(drawCall.primitiveType);

	if (!drawCall.isIndexEnabled()) {
		glDrawArrays(pmt, 0, (GLsizei)drawCall.vertexCount);
	} else {
		if (!drawCall.indexBuffer) throw axError_Undefined(AX_LOC);
		GLenum indexType = Util::getGLIndexType(drawCall.indexType);
		glDrawElements(pmt, (GLsizei)drawCall.indexCount, indexType, (void*)(drawCall.indexByteOffset));
	}
	Util::checkGLError(AX_LOC);
}

axGLCommandDispatcher::BindShaderPass::BindShaderPass(axGLCommandDispatcher& dispatcher_, axRenderDrawCall& drawCall_, axGLMaterial* material_) 
	: dispatcher(&dispatcher_)
	, drawCall(drawCall_)
	, material(material_)
{
	auto* vb = ax_type_cast<axGLGpuBuffer>(drawCall.vertexBuffer.ptr());
	if (!vb) throw axError_Undefined(AX_LOC);
	vb->bind();

	if (drawCall.indexBuffer) {
		auto* ib = ax_type_cast<axGLGpuBuffer>(drawCall.indexBuffer.ptr());
		if (!ib) throw axError_Undefined(AX_LOC);
		ib->bind();
	}
	material->bind(*dispatcher, drawCall);
}

axGLCommandDispatcher::BindShaderPass::~BindShaderPass() {
	material->unbind(*dispatcher, drawCall);
	if (auto* vb = ax_type_cast<axGLGpuBuffer>(drawCall.vertexBuffer.ptr())) {
		vb->unbind();
	}

	if (drawCall.indexBuffer) {
		if (auto* ib = ax_type_cast<axGLGpuBuffer>(drawCall.indexBuffer.ptr())) {
			ib->unbind();
		}
	}
}

#endif