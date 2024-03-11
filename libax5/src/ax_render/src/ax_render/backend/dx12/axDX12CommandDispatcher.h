#pragma once

#if AX_USE_DX12

#include "../../axRenderCommandDispatcher.h"
#include "axDX12DescripterHeap.h"
#include "axDX12CommonBase.h"
#include "axDX12RayTracing.h"
#include "axDX12RenderRequest.h"

class axDX12CommandDispatcher : public axRenderCommandDispatcherImpl<axDX12CommandDispatcher> {
	using Base = axRenderCommandDispatcherImpl<axDX12CommandDispatcher>;
public:
	using Util = axDX12Util;

	axDX12CommandDispatcher(axDX12RenderContext* renderContext_, axDX12RenderRequest* renderRequest);

	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	void _execCmdList();

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS;

	axDX12RenderContext*				renderContext = nullptr;
	axDX12_ID3D12Device*				d3dDevice = nullptr;

	axComPtr<ID3D12CommandQueue>		cmdQueue;
	axComPtr<ID3D12CommandQueue>		computeCmdQueue;

	ID3D12GraphicsCommandList*			cmdList = nullptr;
	ID3D12GraphicsCommandList*			computeCmdList = nullptr;

	axDX12DescriptorHandle				renderTarget;
	axDX12DescriptorHandle				depthStencilBuffer;

	axDX12RenderRequest*				renderRequest = nullptr;

	bool _hasComputeCall = false;

#if AX_USE_DX12_DXR
	void buildRayTracingAccelStructs();
	void cmdDrawCallRayTracing	(DrawCall& drawCall);

	axDX12RayTracing*					rayTracing = nullptr;
	axArray<D3D12_RAYTRACING_GEOMETRY_DESC>	rayTracingGeoDescs;
#endif
};

#endif