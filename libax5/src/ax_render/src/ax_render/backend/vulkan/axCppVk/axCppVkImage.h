#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkCommandBuffer;
class axCppVkImage : public axCppVkBase {
public:
	const VkImage& handle() { return _h; }
	operator const VkImage&() { return _h; }

	axCppVkImage() = default;
	axCppVkImage(axCppVkImage&& r) {
		_h = r._h;
		_dev = r._dev;
		_format = r._format;

		r._h = VK_NULL_HANDLE;
		r._dev = nullptr;
		r._format = VK_FORMAT_UNDEFINED;
	}


	~axCppVkImage() { destroy(); }

	void destroy();

	void createImage2D	(axCppVkDevice& dev, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageLayout initialLayout);

	void createDepthStencil(axCppVkDevice& dev, uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_D32_SFLOAT) {
		createImage2D(dev, width, height, format, 
						VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						VK_IMAGE_LAYOUT_UNDEFINED);
	}

	void setLayout(axCppVkCommandBuffer& cmdBuffer, VkImageLayout newLayout, VkPipelineStageFlags newStage);

	VkFormat	format() { return _format; }

	VkMemoryRequirements getMemoryRequirements();
	axCppVkDevice&	device() { return *_dev; }

	VkImageLayout	layout() const { return _layout; }

private:
	void _create(axCppVkDevice& dev, VkImageCreateInfo& info);

	VkImage _h = VK_NULL_HANDLE;
	axCppVkDevice* _dev = nullptr;
	VkFormat _format = VK_FORMAT_UNDEFINED;
	VkImageLayout _layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkPipelineStageFlags _stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
};

#endif