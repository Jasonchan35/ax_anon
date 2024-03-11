#if AX_USE_VULKAN

#include "axVkGpuBuffer.h"
#include "axVkRenderContext.h"
#include "axVkCommandDispatcher.h"

void axVkGpuBuffer::onCreate(const CreateDesc& desc, axInt bufferSize) {
	auto& dev = Util::rootDevice();
	auto size = Util::castVkDeviceSize(bufferSize);

	VkBufferUsageFlags usage = 0;
	VkMemoryPropertyFlags memProps = 0;

	switch (desc.type) {
		case Type::Vertex: {
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}break;
		case Type::Index: {
			usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}break;
		case Type::Const: {
			usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}break;
		case Type::Storage: { // GPU only
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//			memProps = VK_MEMORY_PROPERTY_PROTECTED_BIT; // no CPU access
		}break;
		case Type::Stage: { // GPU to CPU
			 // TODO
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}break;

		default: throw axError_Undefined(AX_LOC);
	}


	_vkBuf.create(dev, size, usage);
	_vkDevMem.createForBuffer(_vkBuf, memProps);
}

void axVkGpuBuffer::onUploadToGpu(axInt offset, axByteSpan data) {
	_vkDevMem.copyData(data.data(), Util::castVkDeviceSize(data.size()), Util::castVkDeviceSize(offset));
}

void axVkGpuBuffer::resourceToKeepInFrame(axVkCommandDispatcher& dispatcher) {
	if (_lastUsedRenderSeqId != dispatcher.renderSeqId()) {
		_lastUsedRenderSeqId = dispatcher.renderSeqId();
		dispatcher.req->resourceToKeep.vkGpuBuffers.emplaceBack(this);
	}
}

#endif
