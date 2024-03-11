#if AX_USE_METAL

#include "axMTLCommonBase.h"
#include "axMTLTexture.h"
#include "axMTLRenderContext.h"
#include "axMTLRenderer.h"

axMTLTexture2D::axMTLTexture2D(CreateDesc& desc) 
	: Base(desc)
{
	auto* mtlDesc = [MTLTextureDescriptor	texture2DDescriptorWithPixelFormat: Util::getMTLPixelFormat(desc.colorType)
							 				width:		Util::castNSUInt(desc.size.x)
											height:		Util::castNSUInt(desc.size.y)
							 				mipmapped:	desc.mipmapCount > 1];

	auto* mtlDevice = Util::mtlDevice();
	_mtlTexture = [mtlDevice newTextureWithDescriptor: mtlDesc];
	{
		auto& src = desc.samplerState;
		auto* samplerDesc = [MTLSamplerDescriptor new];
		samplerDesc.minFilter 		= Util::getMTLTextureMinMagFilter(src.filter);
		samplerDesc.magFilter 		= Util::getMTLTextureMinMagFilter(src.filter);
		samplerDesc.mipFilter 		= Util::getMTLTextureMipFilter(src.filter, desc.mipmapCount);
		samplerDesc.sAddressMode 	= Util::getMTLTextureWrap(src.wrapU);
		samplerDesc.tAddressMode 	= Util::getMTLTextureWrap(src.wrapV);
		samplerDesc.rAddressMode 	= Util::getMTLTextureWrap(src.wrapW);

		_samplerState = [mtlDevice newSamplerStateWithDescriptor:samplerDesc];
	}

	if (desc.uploadRequest) {
		_onUpdateRequest(*desc.uploadRequest);
	}
}

void axMTLTexture2D::onUploadRequest(axMTLCommandDispatcher& dispatcher, UploadRequest& req) {
	_onUpdateRequest(req);
}

void axMTLTexture2D::_onUpdateRequest(const UploadRequest& req) {
	MTLRegion region;
	region.origin.x		= Util::castNSUInt(req.offset.x);
	region.origin.y		= Util::castNSUInt(req.offset.y);
	region.origin.z		= 0;
	region.size.width	= Util::castNSUInt(req.imageInfo.size.x);
	region.size.height	= Util::castNSUInt(req.imageInfo.size.y);
	region.size.depth	= 1;

	[_mtlTexture	replaceRegion:	region
					mipmapLevel:	0
					withBytes:		req.pixelData.data()
					bytesPerRow:	Util::castNSUInt(req.imageInfo.strideInBytes)];
}

axMTLTexture2D::~axMTLTexture2D() {
}

void axMTLTexture2D::bind() const {
}

#endif //AX_USE_METAL
