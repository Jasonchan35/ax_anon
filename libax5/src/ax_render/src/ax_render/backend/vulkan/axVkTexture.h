#pragma once

#if AX_USE_VULKAN

#include "../../texture/axTexture.h"

class axVkTexture2D : public axTexture2D {
	AX_RTTI_CLASS(axVkTexture2D, axTexture2D)
public:
	using Util = axVkUtil;

	axVkTexture2D(CreateDesc& desc);
	void onUploadRequest(axVkCommandDispatcher& dispatcher, UploadRequest& req);

	~axVkTexture2D();

	class GpuUploadRequest : public axSListNode<GpuUploadRequest> {
	public:
		axSPtr<axVkTexture2D>	tex;
		axVec2i					offset{0,0};
		axImageInfo				imageInfo;
		axVkStagingBuffer		stagingBuffer;
	};

	void _newUploadRequest(axCppVkDevice& device, UploadRequest& req);
	void _onManagerGpuUploadRequest(axVkCommandDispatcher& dispatcher, GpuUploadRequest& req);

	axCppVkDeviceMemory	_imageDevMem;
	axCppVkImage		_image;
	axCppVkImageView	_imageView;
	axCppVkSampler		_sampler;

	axInt		_width = 0;
	axInt		_height = 0;
	ColorType	_colorType = ColorType::Unknown;
};

class axVkTextureManager : public axNonCopyable {
public:
	using This = axVkTextureManager;
public:
	using Tex2D = axVkTexture2D;

	static axVkTextureManager* s_instance();

	void addGpuUploadRequest(axUPtr<Tex2D::GpuUploadRequest> && r) {
		auto q = _pendingUploadRequests2d.scopedLock();
		q->append(r.detach());
	}

	void onGpuFrameBegin(axVkCommandDispatcher& dispatcher);
	void onGpuFrameEnd(axVkCommandDispatcher& dispatcher);

private:
	using GpuUploadRequests2D = axSList<Tex2D::GpuUploadRequest>;

	axMutexProtected<GpuUploadRequests2D>	_pendingUploadRequests2d;
	GpuUploadRequests2D						_uploadRequests2d;

};

#endif //AX_USE_VULKAN
