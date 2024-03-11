#if AX_USE_VULKAN

#include "axVkShaderPass.h"
#include "axVkMaterial.h"
#include "axVkMaterialPass.h"
#include "axVkGpuBuffer.h"
#include "axVkStorageBuffer.h"
#include "axVkStageBuffer.h"
#include "axVkCommandDispatcher.h"
#include "axVkTexture.h"

void axVkMaterialPass::bind(axVkCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	shaderPass_->bind(dispatcher, drawCall);
	_bindParams(dispatcher);
}

void axVkMaterialPass::bind(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	shaderPass_->bind(dispatcher, computeCall);
	_bindParams(dispatcher);
}

void axVkMaterialPass::_bindParams(axVkCommandDispatcher& dispatcher) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	if (!_descPool) {
		axArray<VkDescriptorPoolSize, axRender_kMaxFrameAheadCount> poolSizes;

		if (_uniformBuffers.size()) {
			auto& dst = poolSizes.emplaceBack();
			dst.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dst.descriptorCount = Util::castUInt32(_uniformBuffers.size() * axRender_kMaxFrameAheadCount);
		}

		if (_texParams.size()) {
			auto& dst = poolSizes.emplaceBack();
			dst.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			dst.descriptorCount = Util::castUInt32(_texParams.size() * axRender_kMaxFrameAheadCount);
		}

		if (_storageBufParams.size()) {
			auto& dst = poolSizes.emplaceBack();
			dst.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			dst.descriptorCount = Util::castUInt32(_storageBufParams.size() * axRender_kMaxFrameAheadCount);
		}

		_descPool.create(*dispatcher.device, poolSizes, axRender_kMaxFrameAheadCount);
		_descPool.allocDescriptorSets(_descSets, axRender_kMaxFrameAheadCount, shaderPass_->psoDescSetLayout());

		_descSetIndex = _descSets.size() - 1;
	}

	if (_descSets.size() <= 0) throw axError_Undefined(AX_LOC);

	bool updateNeeded = false;
	if (_lastUsedRenderSeqId != dispatcher.renderSeqId()) {
		_lastUsedRenderSeqId = dispatcher.renderSeqId();
		_descSetIndex = (_descSetIndex + 1) % _descSets.size();

		dispatcher.req->resourceToKeep.vkMaterialPasses.emplaceBack(axSPtr_make(this));
		updateNeeded = true;
	}

	VkDescriptorSet descSet = _descSets[_descSetIndex];
	if (updateNeeded) {
		_updateDescriptorSet(descSet, dispatcher);
	}

	dispatcher.cmdBuffer->setCheckPoint("bind desc set");
	vkCmdBindDescriptorSets(*dispatcher.cmdBuffer, 
							shaderPass_->isCompute() ? VK_PIPELINE_BIND_POINT_COMPUTE :  VK_PIPELINE_BIND_POINT_GRAPHICS, 
							shaderPass_->psoLayout(), 0, 1, &descSet, 0, nullptr);
}


void axVkMaterialPass::_updateDescriptorSet(VkDescriptorSet descSet, axVkCommandDispatcher& dispatcher) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	axArray<VkDescriptorBufferInfo, 32> uniformBufferInfos;
	uniformBufferInfos.resize(_uniformBuffers.size());

	axArray<VkDescriptorImageInfo, 32>  imageInfos;
	imageInfos.resize(_texParams.size());

	axArray<VkDescriptorBufferInfo, 32> storageBufInfos;
	storageBufInfos.resize(_storageBufParams.size());

	axArray<VkWriteDescriptorSet, 32>   writeDescSets;

	{ //const buffer
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			auto* gpuBuf = ax_type_cast<axVkGpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			auto& bi	= uniformBufferInfos[cbIndex];
			bi.offset	= 0;
			bi.range	= Util::castUInt32(cb.dataSize());
			bi.buffer	= gpuBuf->vkBuf();

			auto& w				= writeDescSets.emplaceBack();
			w.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			w.pNext				= nullptr;
			w.dstSet			= descSet;
			w.dstBinding		= Util::castUInt32(cb.bindPoint());
			w.dstArrayElement	= 0;
			w.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			w.descriptorCount	= 1;
			w.pBufferInfo		= &bi;
		}
	}

	{ //storage buffer
		axInt index = 0;
		for (auto& sbufParam : _storageBufParams) {
			auto* sbuf = ax_type_cast<axVkStorageBuffer>(sbufParam.storageBuffer());
			if (!sbuf) throw axError_Undefined(AX_LOC);

			auto& bi	= storageBufInfos[index];
			bi.offset	= 0;
			bi.range	= Util::castUInt32(sbuf->bufferSize());
			bi.buffer	= sbuf->vkBuf();

			auto& w				= writeDescSets.emplaceBack();
			w.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			w.pNext				= nullptr;
			w.dstSet			= descSet;
			w.dstBinding		= Util::castUInt32(sbufParam.bindPoint());
			w.dstArrayElement	= 0;
			w.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			w.descriptorCount	= 1;
			w.pBufferInfo		= &bi;
		}
	}

	{ //bind texture
		axInt texIndex = 0;
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			auto& img = imageInfos[texIndex];

			auto& w = writeDescSets.emplaceBack();
			w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			w.pNext = nullptr;
			w.dstSet = descSet;
			w.dstBinding = Util::castUInt32(texParam.bindPoint());
			w.dstArrayElement = 0;
			w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			w.descriptorCount = 1;
			w.pImageInfo = &img;

			dispatcher.req->resourceToKeep.textures.emplaceBack(axSPtr_make(tex));

			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axVkTexture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);

					img.imageView	= tex2d->_imageView;
					img.imageLayout	= tex2d->_image.layout();
					img.sampler		= tex2d->_sampler;
				}break;

				default: throw axError_Undefined(AX_LOC);
			}
			texIndex++;
		}
	}

	vkUpdateDescriptorSets(*dispatcher.device, Util::castUInt32(writeDescSets.size()), writeDescSets.data(), 0, nullptr);
}

#endif
