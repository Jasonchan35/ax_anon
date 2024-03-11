#if AX_USE_DX11

#include "axDX11Renderer.h"
#include "axDX11CommandDispatcher.h"
#include "axDX11RenderContext.h"
#include "axDX11Fence.h"
#include "axDX11Texture.h"
#include "axDX11Material.h"
#include "axDX11StorageBuffer.h"
#include "axDX11StageBuffer.h"

axDX11CommandDispatcher::axDX11CommandDispatcher(axDX11RenderContext* renderContext_, axRenderRequest* renderRequest_) 
	: Base(renderContext_, renderRequest_)
{
	renderRequest = renderRequest_;
	renderContext = renderContext_;
	d3dContext = renderContext_->renderer()->d3dContext();
	d3dDevice = axDX11Renderer::s_instance()->d3dDevice();
}

void axDX11CommandDispatcher::onUploadTexture2D(axTexture2D_UploadRequest& req) {
	auto* tex = ax_type_cast<axDX11Texture2D>(req.tex.ptr());
	if (!tex) {
		throw axError_Undefined(AX_LOC);
	}
	tex->onUploadRequest(*this, req);
}

void axDX11CommandDispatcher::onGpuFrameBegin() {
	Base::onGpuFrameBegin();
}

void axDX11CommandDispatcher::onGpuFrameEnd() {
	Base::onGpuFrameEnd();
}

void axDX11CommandDispatcher::cmdSwapBuffers(axRenderCommand_SwapBuffers& cmd) {
	renderContext->cmdSwapBuffers();

	auto* fence = ax_type_cast_debug<axDX11Fence>(renderRequest->completedFence());
	fence->addToGpu(d3dContext);
}

void axDX11CommandDispatcher::cmdClearColorAndDepthBuffers(axRenderCommand_ClearColorAndDepthBuffers& cmd) {
	if (_renderTargetView) {
		d3dContext->ClearRenderTargetView(_renderTargetView, cmd.color.data());
	}
	if (_depthStencilView) {
		d3dContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, cmd.depth, 0);
	}
}

void axDX11CommandDispatcher::cmdSetViewport(axRenderCommand_SetViewport& cmd) {
	D3D11_VIEWPORT vp;
	vp.TopLeftX = cmd.viewport.x;
	vp.TopLeftY = cmd.viewport.y;
	vp.Width	= cmd.viewport.w;
	vp.Height	= cmd.viewport.h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	d3dContext->RSSetViewports(1, &vp);
}

void axDX11CommandDispatcher::cmdDrawCall(axRenderCommand_DrawCall& cmd) {
	auto& drawCall = cmd.drawCall;
	auto* vb = ax_type_cast_debug<axDX11GpuBuffer>(drawCall.vertexBuffer.ptr());
	if (!vb) throw axError_Undefined(AX_LOC);

	auto* material = ax_type_cast_debug<axDX11Material>(drawCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	auto bindMtl = material->scopedBind(*this, drawCall);

	{
		axRect2f rc = drawCall.scissorWorldRect - renderContext->worldPos();
		D3D11_RECT d3dRect;
		d3dRect.left   = static_cast<LONG>(rc.xMin());
		d3dRect.top    = static_cast<LONG>(rc.yMin());
		d3dRect.right  = static_cast<LONG>(rc.xMax());
		d3dRect.bottom = static_cast<LONG>(rc.yMax());

		d3dContext->RSSetScissorRects(1, &d3dRect);
	}

	auto vertexDesc = drawCall.vertexDesc;
	if (!vertexDesc) return;

//vertex buffer
	UINT strideBytes = vertexDesc->strideBytes;
	UINT vertexByteOffset =	Util::castUINT(drawCall.vertexByteOffset);
	UINT indexByteOffset  =	Util::castUINT(drawCall.indexByteOffset);
	
	ID3D11Buffer* pVb = vb->_d3dBuf;
	d3dContext->IASetVertexBuffers(0, 1, &pVb, &strideBytes, &vertexByteOffset);

	auto pmt = Util::getDxPrimitiveTopology(drawCall.primitiveType);

	d3dContext->IASetPrimitiveTopology(pmt);

	if (!drawCall.isIndexEnabled()) {
		d3dContext->Draw(Util::castUINT(drawCall.vertexCount), 0);
	}else{
		if (!drawCall.indexBuffer) throw axError_Undefined(AX_LOC);
		auto* ib = ax_type_cast_debug<axDX11GpuBuffer>(drawCall.indexBuffer.ptr());
		if (!ib) throw axError_Undefined(AX_LOC);

		auto indexType = Util::getDxIndexType(drawCall.indexType);
		ID3D11Buffer* pIb = ib->_d3dBuf;
		d3dContext->IASetIndexBuffer(pIb, indexType, indexByteOffset);
		d3dContext->DrawIndexed(Util::castUINT(drawCall.indexCount), 0, 0);
	}
}

void axDX11CommandDispatcher::cmdComputeCall(axRenderCommand_ComputeCall& cmd) {
	auto* material = ax_type_cast_debug<axDX11Material>(cmd.computeCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	auto bindMtl = material->scopedBind(*this, cmd.computeCall);

	auto tc = axVec3<UINT>::s_cast(cmd.computeCall.threadGroupCount);
	d3dContext->Dispatch(tc.x, tc.y, tc.z);
}

void axDX11CommandDispatcher::cmdCopyToStageBuffer(axRenderCommand_CopyToStageBuffer& cmd) {
	auto* dst = ax_type_cast_debug<axDX11StageBuffer  >(cmd.dst.ptr())->gpuBuffer();
	auto* src = ax_type_cast_debug<axDX11StorageBuffer>(cmd.src.ptr())->gpuBuffer();

	if (!dst || !src) throw axError_Undefined(AX_LOC);

	d3dContext->CopyResource(dst->_d3dBuf, src->_d3dBuf);
}

#endif
