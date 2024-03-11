#if AX_USE_DX12

#include "axDX12CommandDispatcher.h"
#include "axDX12RenderContext.h"
#include "axDX12Material.h"
#include "axDX12StorageBuffer.h"
#include "axDX12StageBuffer.h"
#include "axDX12Texture.h"
#include "axDX12RayTracing.h"
#include "axDX12Fence.h"

axDX12CommandDispatcher::axDX12CommandDispatcher(axDX12RenderContext* renderContext_, axDX12RenderRequest* renderRequest_) 
	: Base(renderContext_, renderRequest_)
	, renderContext(renderContext_)
	, renderRequest(renderRequest_)
{
	d3dDevice = Util::d3dDevice();

	auto hr = renderRequest->_cmdAllocator->Reset();
	Util::throwIfError(hr);

	cmdList = renderRequest->_cmdList;
	hr = cmdList->Reset(renderRequest->_cmdAllocator, nullptr);
	Util::throwIfError(hr);

	computeCmdList = renderRequest->_computeCmdList;
	hr = computeCmdList->Reset(renderRequest->_computeCmdAllocator, nullptr);
	Util::throwIfError(hr);
}

void axDX12CommandDispatcher::_execCmdList() {
	AX_ZoneScoped;

	auto hr = computeCmdList->Close();
	Util::throwIfError(hr);
	
	hr = cmdList->Close();
	Util::throwIfError(hr);

	if (_hasComputeCall) {
		cmdQueue->Signal(renderRequest->_computeFence, 0);
		{
			ID3D12CommandList* list = {computeCmdList};
			computeCmdQueue->ExecuteCommandLists(1, &list);
			computeCmdQueue->Signal(renderRequest->_computeFence, 1);
		}
		cmdQueue->Wait(renderRequest->_computeFence, 1);
	}

	{
		ID3D12CommandList* list = {cmdList};
		cmdQueue->ExecuteCommandLists(1, &list);
	}
}

void axDX12CommandDispatcher::onUploadTexture2D(axTexture2D_UploadRequest& req) {
	auto* tex = ax_type_cast<axDX12Texture2D>(req.tex.ptr());
	if (!tex) {
		throw axError_Undefined(AX_LOC);
	}
	tex->onUploadRequest(*this, req);
}

void axDX12CommandDispatcher::onGpuFrameBegin() {
	Base::onGpuFrameBegin();
	auto* texMgr = axDX12TextureManager::s_instance();
	texMgr->onGpuFrameBegin(*this);
}

void axDX12CommandDispatcher::onGpuFrameEnd() {
	auto* texMgr = axDX12TextureManager::s_instance();
	texMgr->onGpuFrameEnd(*this);

	Base::onGpuFrameEnd();
}

void axDX12CommandDispatcher::cmdSwapBuffers(axRenderCommand_SwapBuffers& cmd) {
#if AX_USE_DX12_DXR
	if (rayTracing) {
		buildRayTracingAccelStructs();
		ID3D12CommandList* cmd = rayTracing->dxrCmdList();
		rayTracing->cmdQueue()->ExecuteCommandLists(1, &cmd);
	}
#endif
	renderContext->cmdSwapBuffers(*this);
}

void axDX12CommandDispatcher::cmdComputeCall(axRenderCommand_ComputeCall& cmd) {
	auto* material = ax_type_cast<axDX12Material>(cmd.computeCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(computeCmdList, cmd.computeCall);

	auto tc = axVec3<UINT>::s_cast(cmd.computeCall.threadGroupCount);
	computeCmdList->Dispatch(tc.x, tc.y, tc.z);
	_hasComputeCall = true;
}

void axDX12CommandDispatcher::cmdCopyToStageBuffer(axRenderCommand_CopyToStageBuffer& cmd) {
	auto* dst = ax_type_cast<axDX12StageBuffer	>(cmd.dst.ptr());
	auto* src = ax_type_cast<axDX12StorageBuffer>(cmd.src.ptr());

	if (!dst || !src) throw axError_Undefined(AX_LOC);

	auto* dstRes = dst->d3dResource();
	auto* srcRes = src->d3dResource();

	if (!dstRes || !srcRes) throw axError_Undefined(AX_LOC);

	src->gpuBuffer()->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	dst->gpuBuffer()->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList->CopyResource(dstRes, srcRes);
}

//axDX12GpuBuffer* axDX12CommandDispatcher::buildRayTracingAccelStruct(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc, axDX12RayTracing::AccelStruct& accelStruct) {
//
//	auto* dxrDevice = rayTracing->dxrDevice();
//
//	auto& inputs = desc.Inputs;
//
//	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
//	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
//	inputs.NumDescs = castUINT(rayTracingGeoDescs.size());
//	inputs.pGeometryDescs = rayTracingGeoDescs.data();
//
//	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
//	dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);
//
//	if (prebuildInfo.ResultDataMaxSizeInBytes <= 0)
//		throw axError_Undefined(AX_LOC);
//
//	auto* scratchBuffer = ax_type_cast<axDX12GpuBuffer>(accelStruct.scratchBuffer.nextGpuBuffer(*this));
//	if (!scratchBuffer) throw axError_Undefined(AX_LOC);
//
//	auto* destAccelStruct = ax_type_cast<axDX12GpuBuffer>(accelStruct.accelStruct.nextGpuBuffer(*this));
//	if (!destAccelStruct) throw axError_Undefined(AX_LOC);
//
//	axRenderGpuBuffer_CreateDesc scratchBufferDesc;
//	scratchBufferDesc.type = axRenderBufferType::UAV;
//	scratchBuffer->create(scratchBufferDesc, prebuildInfo.ResultDataMaxSizeInBytes);
//
//	axRenderGpuBuffer_CreateDesc destAccelStructDesc;
//	destAccelStructDesc.type = axRenderBufferType::RayTracingAccelStruct;
//
//	destAccelStruct->create(destAccelStructDesc, prebuildInfo.ResultDataMaxSizeInBytes);
//
//	desc.ScratchAccelerationStructureData = scratchBuffer->gpuAddress();
//	desc.DestAccelerationStructureData    = destAccelStruct->gpuAddress();
//
//	rayTracing->dxrCmdList()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
//
//	return destAccelStruct;
//}

#if AX_USE_DX12_DXR

void axDX12CommandDispatcher::buildRayTracingAccelStructs() {
	if (!rayTracing) return;

	auto* dxrDevice = rayTracing->dxrDevice();

	UINT64 topLevelSize = 0;
	UINT64 bottomLevelSize = 0;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelDesc = {};

	{
		auto& desc = bottomLevelDesc;
		desc.Inputs.Type		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		desc.Inputs.Flags		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		desc.Inputs.DescsLayout	= D3D12_ELEMENTS_LAYOUT_ARRAY;

		desc.Inputs.NumDescs = castUINT(rayTracingGeoDescs.size());
		desc.Inputs.pGeometryDescs = rayTracingGeoDescs.data();

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&desc.Inputs, &prebuildInfo);

		if (prebuildInfo.ResultDataMaxSizeInBytes <= 0)
			throw axError_Undefined(AX_LOC);

		bottomLevelSize = prebuildInfo.ResultDataMaxSizeInBytes;
	}

	{
		auto& desc = topLevelDesc;
		desc.Inputs.Type		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		desc.Inputs.Flags		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		desc.Inputs.DescsLayout	= D3D12_ELEMENTS_LAYOUT_ARRAY;

		desc.Inputs.NumDescs = castUINT(rayTracingGeoDescs.size());
		desc.Inputs.pGeometryDescs = rayTracingGeoDescs.data();

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&desc.Inputs, &prebuildInfo);

		if (prebuildInfo.ResultDataMaxSizeInBytes <= 0)
			throw axError_Undefined(AX_LOC);

		topLevelSize = prebuildInfo.ResultDataMaxSizeInBytes;
	}

// Scratch Buffer
	axDX12GpuBuffer* scratchBuffer			= ax_type_cast<axDX12GpuBuffer>(rayTracing->_scratchBuffer.nextGpuBuffer(*this, ax_max(topLevelSize, bottomLevelSize)));

// Bottom Level
	axDX12GpuBuffer* bottomLevelAccelStruct	= ax_type_cast<axDX12GpuBuffer>(rayTracing->_bottomLevelAccelStruct.nextGpuBuffer(*this, bottomLevelSize));
	{
		bottomLevelDesc.ScratchAccelerationStructureData = scratchBuffer->gpuAddress();
		bottomLevelDesc.DestAccelerationStructureData    = bottomLevelAccelStruct->gpuAddress();

		rayTracing->dxrCmdList()->BuildRaytracingAccelerationStructure(&bottomLevelDesc, 0, nullptr);
	}

// Instance Desc
	axArray<D3D12_RAYTRACING_INSTANCE_DESC, 4> instDescs;
	{
		auto& dst = instDescs.emplaceBack();
		dst.Transform[0][0] = dst.Transform[1][1] = dst.Transform[2][2] = 1;
		dst.InstanceMask = 1;
		dst.AccelerationStructure = bottomLevelAccelStruct->gpuAddress();
	}
	axDX12GpuBuffer* instDescsBuf		= ax_type_cast<axDX12GpuBuffer>(rayTracing->_instanceDescsBuf.nextGpuBuffer(*this, instDescs.sizeInBytes()));
	instDescsBuf->uploadToGpu(instDescs.toByteSpan());

// Top Level
	axDX12GpuBuffer* topLevelAccelStruct	= ax_type_cast<axDX12GpuBuffer>(rayTracing->_topLevelAccelStruct.nextGpuBuffer(*this, topLevelSize));
	{
		topLevelDesc.ScratchAccelerationStructureData = scratchBuffer->gpuAddress();
		topLevelDesc.DestAccelerationStructureData    = topLevelAccelStruct->gpuAddress();

		topLevelDesc.Inputs.NumDescs = castUINT(instDescs.size());
		topLevelDesc.Inputs.InstanceDescs = instDescsBuf->gpuAddress();

		rayTracing->dxrCmdList()->BuildRaytracingAccelerationStructure(&bottomLevelDesc, 0, nullptr);
	}
}

void axDX12CommandDispatcher::cmdDrawCallRayTracing(DrawCall& drawCall) {
	if (!rayTracing)
		return;

	auto& geo = rayTracingGeoDescs.emplaceBack();
	geo.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geo.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geo.Triangles.Transform3x4 = 0;

	{
		auto* vertexDesc = axRenderVertexDesc_Get(drawCall.vertexType);
		if (!drawCall.vertexBuffer) throw axError_Undefined(AX_LOC);
		auto* vb = ax_type_cast<axDX12GpuBuffer>(drawCall.vertexBuffer->gpuBuffer());
		if (!vb) throw axError_Undefined(AX_LOC);

		auto& posAttr = vertexDesc->attr(VertexAttrId::POSITION0);

		geo.Triangles.VertexFormat	= getDxDataType(posAttr.dataType);
		geo.Triangles.VertexCount	= castUINT(drawCall.vertexCount);
		geo.Triangles.VertexBuffer.StartAddress = vb->gpuAddress();
		geo.Triangles.VertexBuffer.StrideInBytes = vertexDesc->strideBytes;
	}

	if (drawCall.indexType == axRenderIndexType::Unknown) {
		geo.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
	} else {
		if (!drawCall.indexBuffer) throw axError_Undefined(AX_LOC);
		auto* ib = ax_type_cast<axDX12GpuBuffer>(drawCall.indexBuffer->gpuBuffer());
		if (!ib) throw axError_Undefined(AX_LOC);

		geo.Triangles.IndexBuffer	= ib->gpuAddress();
		geo.Triangles.IndexCount	= castUINT(drawCall.indexCount);
		geo.Triangles.IndexFormat	= getDxIndexType(drawCall.indexType);
	}
}

#endif

void axDX12CommandDispatcher::cmdClearColorAndDepthBuffers(axRenderCommand_ClearColorAndDepthBuffers& cmd) {
	cmdList->ClearRenderTargetView(renderTarget, cmd.color.data(), 0, nullptr);
	cmdList->ClearDepthStencilView(depthStencilBuffer, D3D12_CLEAR_FLAG_DEPTH, cmd.depth, 0, 0, nullptr);
}

void axDX12CommandDispatcher::cmdSetViewport(axRenderCommand_SetViewport& cmd) {
	auto& rc = cmd.viewport;

	D3D12_VIEWPORT vp;
	vp.TopLeftX = rc.x;
	vp.TopLeftY = rc.y;
	vp.Width	= rc.w;
	vp.Height	= rc.h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	cmdList->RSSetViewports(1, &vp);
}

void axDX12CommandDispatcher::cmdDrawCall(axRenderCommand_DrawCall& cmd) {
	auto& drawCall = cmd.drawCall;
#if AX_USE_DX12_DXR
	cmdDrawCallRayTracing(drawCall);
#endif

	auto topology  = Util::getDxPrimitiveTopology(drawCall.primitiveType);

	cmdList->IASetPrimitiveTopology(topology);

	auto* material = ax_type_cast<axDX12Material>(drawCall.material.ptr());
	if (!material) throw axError_Undefined(AX_LOC);

	material->bind(cmdList, drawCall);

	{
		axRect2f rc = drawCall.scissorWorldRect - renderContext->worldPos();
		D3D12_RECT d3dRect;
		d3dRect.left   = static_cast<LONG>(rc.xMin());
		d3dRect.top    = static_cast<LONG>(rc.yMin());
		d3dRect.right  = static_cast<LONG>(rc.xMax());
		d3dRect.bottom = static_cast<LONG>(rc.yMax());

		cmdList->RSSetScissorRects(1, &d3dRect); // DX12 always enable scissor test
	}

	{ // bind vertex buffer
		auto vertexDesc = drawCall.vertexDesc;
		if (!vertexDesc) throw axError_Undefined(AX_LOC);
		if (!drawCall.vertexBuffer) throw axError_Undefined(AX_LOC);

		auto* vb = ax_type_cast<axDX12GpuBuffer>(drawCall.vertexBuffer.ptr());
		if (!vb) throw axError_Undefined(AX_LOC);

		vb->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		D3D12_VERTEX_BUFFER_VIEW vbView = {};
		vbView.BufferLocation = Util::castUINT64(vb->gpuAddress() + drawCall.vertexByteOffset);
		vbView.SizeInBytes    = Util::castUINT(vb->bufferSize()   - drawCall.vertexByteOffset);
		vbView.StrideInBytes  = vertexDesc->strideBytes;
		cmdList->IASetVertexBuffers(0, 1, &vbView);
	}

	if (!drawCall.isIndexEnabled()) {
		cmdList->DrawInstanced(Util::castUINT(drawCall.vertexCount), 1, 0, 0);
	} else {
		auto indexType = Util::getDxIndexType(drawCall.indexType);
		if (!drawCall.indexBuffer) throw axError_Undefined(AX_LOC);
		auto* ib = ax_type_cast<axDX12GpuBuffer>(drawCall.indexBuffer.ptr());
		if (!ib) throw axError_Undefined(AX_LOC);

		//ib->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		D3D12_INDEX_BUFFER_VIEW ibView = {};
		ibView.BufferLocation = Util::castUINT64(ib->gpuAddress() + drawCall.indexByteOffset);
		ibView.SizeInBytes    = Util::castUINT(ib->bufferSize()   - drawCall.indexByteOffset);
		ibView.Format = indexType;
		cmdList->IASetIndexBuffer(&ibView);
		cmdList->DrawIndexedInstanced(Util::castUINT(drawCall.indexCount), 1, 0, 0, 0);
	}
}

#endif
