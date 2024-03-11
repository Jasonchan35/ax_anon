#if AX_USE_VULKAN

#include "axVkTexture.h"
#include "axVkRenderContext.h"
#include "axVkRenderer.h"
#include "axVkCommonBase.h"
#include "axVkCommandDispatcher.h"

axVkTexture2D::axVkTexture2D(CreateDesc& desc) 
	: Base(desc)
{
	{
		auto& ss = desc.samplerState;

		VkSamplerCreateInfo info = {};
		info.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext						= nullptr;
		info.magFilter					= Util::getVkTextureFilter(ss.filter);
		info.minFilter					= Util::getVkTextureFilter(ss.filter);
		info.mipmapMode					= Util::getVkSamplerMipmapMode(ss.filter);
		info.addressModeU				= Util::getVkTextureWrap(ss.wrapU);
		info.addressModeV				= Util::getVkTextureWrap(ss.wrapV);
		info.addressModeW				= Util::getVkTextureWrap(ss.wrapW);
		info.mipLodBias					= 0.0f;
		info.anisotropyEnable			= VK_FALSE;
		info.maxAnisotropy				= 1;
		info.compareOp					= VK_COMPARE_OP_NEVER;
		info.minLod						= 0.0f;
		info.maxLod						= 0.0f;
		info.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		info.unnormalizedCoordinates	= VK_FALSE;

		_sampler.create(Util::rootDevice(), info);
	}

	if (desc.uploadRequest) {
		_newUploadRequest(Util::rootDevice(), *desc.uploadRequest);
	}
}

void axVkTexture2D::onUploadRequest(axVkCommandDispatcher& dispatcher, UploadRequest& req) {
	_newUploadRequest(*dispatcher.device, req);
}

void axVkTexture2D::_newUploadRequest(axCppVkDevice& device, UploadRequest& req) {
	auto p = axUPtr_new(GpuUploadRequest());
	p->tex.ref(this);
	p->imageInfo = req.imageInfo;

	auto& data = req.pixelData;
	p->stagingBuffer._buf.create(device, Util::castVkDeviceSize(data.sizeInBytes()), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	p->stagingBuffer._devMem.createForBuffer(p->stagingBuffer._buf, memProps);
	p->stagingBuffer._devMem.copyData(data.data(), Util::castVkDeviceSize(data.sizeInBytes()));

	auto* mgr = axVkTextureManager::s_instance();
	mgr->addGpuUploadRequest(ax_move(p));
}


void axVkTexture2D::_onManagerGpuUploadRequest(axVkCommandDispatcher& dispatcher, GpuUploadRequest& req) {
	auto& info = req.imageInfo;

	auto width  = Util::castUInt32(info.size.x);
	auto height = Util::castUInt32(info.size.y);

	if (!_image || _width != info.size.x || _height != info.size.y || _colorType != info.colorType) {
		if (_image) {
			dispatcher.req->resourceToKeep.vkImages.emplaceBack(ax_move(_image));
			dispatcher.req->resourceToKeep.vkImageViews.emplaceBack(ax_move(_imageView));
		}

		_image.createImage2D(	*dispatcher.device,
								width,
								height,
								Util::getVkColorType(info.colorType),
								VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
								VK_IMAGE_LAYOUT_PREINITIALIZED);
		_imageDevMem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		_imageView.create(_image);
	}

	//{
	//	VkBufferMemoryBarrier barrier = {};
	//	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	//	barrier.pNext = nullptr;
	//	barrier.srcAccessMask = 0;
	//	barrier.dstAccessMask = 0;
	//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.buffer = stagingBuf._buf;
	//	barrier.offset = 0;
	//	barrier.size   = castVkDeviceSize(data.sizeInBytes());

	//	vkCmdPipelineBarrier(	*dispatcher.cmdBuffer,
	//							VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
	//							VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
	//							0,
	//							0, nullptr, // memory barrier
	//							1, &barrier, // buffer barrier
	//							0, nullptr);
	//}

	VkBufferImageCopy region = {};
	region.bufferOffset						= 0;
	region.bufferRowLength					= width;
	region.bufferImageHeight				= height;
	region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel		= 0;
    region.imageSubresource.baseArrayLayer	= 0;
    region.imageSubresource.layerCount		= 1;
	region.imageOffset.x		= 0;
	region.imageOffset.y		= 0;
	region.imageOffset.z		= 0;
	region.imageExtent.width	= width;
	region.imageExtent.height	= height;
	region.imageExtent.depth	= 1;

	_image.setLayout(*dispatcher.cmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);

	dispatcher.cmdBuffer->setCheckPoint("CopyBufferToImage");
	vkCmdCopyBufferToImage(*dispatcher.cmdBuffer, req.stagingBuffer._buf, _image, _image.layout(), 1, &region);

	_image.setLayout(*dispatcher.cmdBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

	dispatcher.req->resourceToKeep.stagingBuffers.emplaceBack(ax_move(req.stagingBuffer));
}

axVkTexture2D::~axVkTexture2D() {
	ax_Vulkan_TODO
}

axVkTextureManager* axVkTextureManager::s_instance() {
	static axGlobalSingleton<This> s;
	return s.ptr();
}

void axVkTextureManager::onGpuFrameBegin(axVkCommandDispatcher& dispatcher) {
	{
		auto q = _pendingUploadRequests2d.scopedLock();
		_uploadRequests2d = ax_move(*q);
	}

	for (auto& t : _uploadRequests2d) {
		t.tex->_onManagerGpuUploadRequest(dispatcher, t);
	}
}

void axVkTextureManager::onGpuFrameEnd(axVkCommandDispatcher& dispatcher) {
	_uploadRequests2d.clear();
}

#endif //AX_USE_VULKAN
