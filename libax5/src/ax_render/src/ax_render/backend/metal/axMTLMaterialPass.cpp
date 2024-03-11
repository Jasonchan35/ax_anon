#include "axMTLMaterialPass.h"
#include "axMTLStorageBuffer.h"
#include "axMTLStageBuffer.h"

#if AX_USE_METAL

void axMTLMaterialPass::bind(axMTLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	shaderPass_->bind(dispatcher, drawCall);
	_bindParams(dispatcher.cmdEncoder);
}

void axMTLMaterialPass::bind(axMTLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	shaderPass_->bind(dispatcher, computeCall);

	_bindParams(dispatcher.computeCmdEncoder);
}

void axMTLMaterialPass::_bindParams(id<MTLRenderCommandEncoder>  cmdEncoder) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	{ //const buffer
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			auto* gpuBuf = ax_type_cast_debug<axMTLGpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			auto bindPoint = Util::castNSUInt(cb.bindPoint());
			auto stage = ax_enum(cb.stageFlags());
			if (stage.hasFlags(StageFlags::Vertex)) {
				[cmdEncoder setVertexBuffer:  gpuBuf->mtlBuffer() offset:0 atIndex:bindPoint];
			}
			if (stage.hasFlags(StageFlags::Pixel)) {
				[cmdEncoder setFragmentBuffer:gpuBuf->mtlBuffer() offset:0 atIndex:bindPoint];
			}
			cbIndex++;
		}
	}

	{ //bind texture
		axInt texIndex = 0;
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axMTLTexture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);

					auto bindPoint = Util::castNSUInt(texParam.bindPoint());
					auto stage = ax_enum(texParam.stageFlags());
					if (stage.hasFlags(StageFlags::Vertex)) {
						[cmdEncoder setVertexTexture:   	tex2d->_mtlTexture   atIndex: bindPoint];
						[cmdEncoder setVertexSamplerState:	tex2d->_samplerState atIndex: bindPoint];
					}

					if (stage.hasFlags(StageFlags::Pixel)) {
						[cmdEncoder setFragmentTexture: 		tex2d->_mtlTexture   atIndex: bindPoint];
						[cmdEncoder setFragmentSamplerState:	tex2d->_samplerState atIndex: bindPoint];
					}
				}break;

				default: throw axError_Undefined(AX_LOC);
			}
			texIndex++;
		}
	}
}

void axMTLMaterialPass::_bindParams(id<MTLComputeCommandEncoder> cmdEncoder) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	{ //const buffer
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			auto* gpuBuf = ax_type_cast_debug<axMTLGpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			auto bindPoint = Util::castNSUInt(cb.bindPoint());
			auto stage = ax_enum(cb.stageFlags());

			auto* mtlBuf = gpuBuf->mtlBuffer();
			if (!mtlBuf) throw axError_Undefined(AX_LOC);

			if (stage.hasFlags(StageFlags::Compute)) {
				[cmdEncoder setBuffer:mtlBuf offset:0 atIndex:bindPoint];
			}
			cbIndex++;
		}
	}

	{ //bind texture
		axInt texIndex = 0;
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axMTLTexture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);

					auto bindPoint = Util::castNSUInt(texParam.bindPoint());
					auto stage = ax_enum(texParam.stageFlags());

					if (stage.hasFlags(StageFlags::Compute)) {
						[cmdEncoder setTexture: 		tex2d->_mtlTexture   atIndex: bindPoint];
						[cmdEncoder setSamplerState:	tex2d->_samplerState atIndex: bindPoint];
					}
				}break;

				default: throw axError_Undefined(AX_LOC);
			}
			texIndex++;
		}
	}

	{ // bind storage buffers
		axInt sbufIndex = 0;
		for (auto& sbufParam : _storageBufParams) {
			auto* sbuf = ax_type_cast_debug<axMTLStorageBuffer>(sbufParam.storageBuffer());
			if (!sbuf) {
				AX_DUMP_VAR(shaderPass_->paramMap());
				throw axError_Undefined(AX_LOC);
			}

			auto* gpuBuf = sbuf->gpuBuffer();
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			auto bindPoint = Util::castNSUInt(sbufParam.bindPoint());
			auto stage = ax_enum(sbufParam.stageFlags());

			auto* mtlBuf = gpuBuf->mtlBuffer();
			if (!mtlBuf) throw axError_Undefined(AX_LOC);

			if (stage.hasFlags(StageFlags::Compute)) {
				[cmdEncoder setBuffer:mtlBuf offset:0 atIndex:bindPoint];
			}

			sbufIndex++;
		}
	}
}

#endif
