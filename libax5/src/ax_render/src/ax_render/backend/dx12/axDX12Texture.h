#pragma once

#if AX_USE_DX12

#include <ax_render/ax_render_config.h>
#include <ax_render/texture/axTexture.h>
#include "axDX12CommonBase.h"
#include "axDX12Resource.h"

class axDX12Renderer;
class axRenderContext;

class axDX12Texture2D : public axTexture2D {
	AX_RTTI_CLASS(axDX12Texture2D, axTexture2D);
public:
	using Util = axDX12Util;

	axDX12Texture2D(CreateDesc& desc);

	void onUploadRequest(axDX12CommandDispatcher& dispatcher, UploadRequest& req);

	class GpuUploadRequest : public axSListNode<GpuUploadRequest> {
	public:
		axSPtr<axDX12Texture2D> tex;
		axVec2i					offset{0,0};
		axImageInfo				imageInfo;
		axDX12Resource_Buffer	stagingBuffer;
	};
	
	axDX12Resource_Texture2D	_tex;
	SamplerState				_samplerState;

	void _newUploadRequest(UploadRequest& req);
	void _onManagerGpuUploadRequest(axDX12CommandDispatcher& dispatcher, GpuUploadRequest& req);
};

class axDX12TextureManager : public axNonCopyable {
	using This = axDX12TextureManager;
public:
	using Tex2D = axDX12Texture2D;

	static axDX12TextureManager* s_instance();

	void addGpuUploadRequest(axUPtr<Tex2D::GpuUploadRequest> && r) {
		auto q = _pendingUploadRequests2d.scopedLock();
		q->append(r.detach());
	}

	void onGpuFrameBegin(axDX12CommandDispatcher& dispatcher);
	void onGpuFrameEnd(axDX12CommandDispatcher& dispatcher);

	using GpuUploadRequests2D = axSList<Tex2D::GpuUploadRequest>;

private:
	axMutexProtected<GpuUploadRequests2D>	_pendingUploadRequests2d;
};

#endif //AX_USE_DX12
