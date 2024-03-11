#if AX_USE_DX12

#include "axDX12Texture.h"
#include "axDX12RenderContext.h"
#include "axDX12CommandDispatcher.h"

axDX12Texture2D::axDX12Texture2D(CreateDesc& desc)
	: Base(desc)
{
	_samplerState = desc.samplerState;
	_tex.create(desc.size, desc.mipmapCount, desc.colorType);

	if (desc.uploadRequest) {
		_newUploadRequest(*desc.uploadRequest);
	}
}

void axDX12Texture2D::onUploadRequest(axDX12CommandDispatcher& dispatcher, UploadRequest& req) {
	_newUploadRequest(req);
}

void axDX12Texture2D::_newUploadRequest(UploadRequest& req) {
	auto p = axUPtr_new(GpuUploadRequest());
	p->tex.ref(this);

	auto pixelData = req.pixelData;
	auto alignment = axInt(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	auto stride = req.imageInfo.strideInBytes;

	axImage tmpImage;
	if (stride % alignment) {
		stride = ax_multiple_of(stride, alignment);
		auto& src = req.imageInfo;
		tmpImage.create(src.colorType, src.size.x, src.size.y, stride);
		tmpImage.subImage({0,0}, req.imageInfo, req.pixelData);
		p->imageInfo = tmpImage.info();
		pixelData = tmpImage.pixelData();

	} else {
		p->imageInfo = req.imageInfo;
	}

	p->stagingBuffer.create(axRenderBufferType::Upload, pixelData);

	auto* mgr = axDX12TextureManager::s_instance();
	mgr->addGpuUploadRequest(ax_move(p));
}

void axDX12Texture2D::_onManagerGpuUploadRequest(axDX12CommandDispatcher& dispatcher, GpuUploadRequest& req) {
	AX_ASSERT(req.tex.ptr() == this);
	auto* dstTex = _tex.d3dResource();
	if (!dstTex) 
		throw axError_Undefined(AX_LOC);

	auto desc = dstTex->GetDesc();

	axRect2i rect(req.offset, req.imageInfo.size);

	D3D12_BOX box;
	box.left   = Util::castUINT(rect.xMin());
	box.right  = Util::castUINT(rect.xMax());
	box.top    = Util::castUINT(rect.yMin());
	box.bottom = Util::castUINT(rect.yMax());
	box.front  = 0;
	box.back   = 1;

	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.pResource = req.stagingBuffer.d3dResource();
	src.PlacedFootprint.Offset = 0;

	auto& fp = src.PlacedFootprint.Footprint;
	fp.Format = Util::getDxColorType(req.imageInfo.colorType);
	fp.Depth = 1;
	fp.RowPitch = Util::castUINT(req.imageInfo.strideInBytes);

	if (fp.RowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) {
		AX_LOG("Error: Texture RowPitch must be align to {?}", D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		throw axError_Undefined(AX_LOC);
	}

	fp.Width    = Util::castUINT(rect.w);
	fp.Height   = Util::castUINT(rect.h);

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.pResource = dstTex;
	dst.SubresourceIndex = src.SubresourceIndex  = 0;

	{
		D3D12_RESOURCE_BARRIER	barrier = {};
		barrier.Transition.pResource = dstTex;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.Subresource = 0;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}

	dispatcher.cmdList->CopyTextureRegion(&dst, box.left, box.top, box.front, &src, &box);

	{
		D3D12_RESOURCE_BARRIER	barrier = {};
		barrier.Transition.pResource = dstTex;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = 0;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}
}

axDX12TextureManager* axDX12TextureManager::s_instance() {
	static axGlobalSingleton<This> s;
	return s.ptr();
}

void axDX12TextureManager::onGpuFrameBegin(axDX12CommandDispatcher& dispatcher) {
	{
		auto q = _pendingUploadRequests2d.scopedLock();
		dispatcher.renderRequest->_textureUploadRequests2d = ax_move(*q);
	}

	for (auto& t : dispatcher.renderRequest->_textureUploadRequests2d) {
		t.tex->_onManagerGpuUploadRequest(dispatcher, t);
	}
}

void axDX12TextureManager::onGpuFrameEnd(axDX12CommandDispatcher& dispatcher) {
}

#endif //AX_USE_DX12
