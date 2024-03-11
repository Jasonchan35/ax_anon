#include "axMaterialPass.h"
#include "../axRenderContext.h"
#include "../buffer/axRenderMultiGpuBuffer.h"
#include "../axRenderBuiltInAssets.h"

void axMaterialPass::setShaderPass(axShaderPass* newShaderPass) {
	_shaderPass.ref(newShaderPass);

	{ //const buffer
		auto& shdCbMap = newShaderPass->uniformBufferMap();
		_uniformBuffers.resize(shdCbMap.size());

		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			auto& src = shdCbMap[cbIndex];
			cb._paramValues	= src.defaultParamValues;
			cb._bindPoint	= src.bindPoint;
			cb._stageFlags	= src.stageFlags;
			cb._glLoc		= src._glLoc;
			cbIndex++;
		}
	}

	{ // tex params
		auto& shdTexParams = newShaderPass->texParams();
		auto& shdParamMap  = newShaderPass->paramMap();

		axInt texIndex = 0;
		_texParams.resize(shdTexParams.size());
		for (auto& texParam : _texParams) {
			auto& src   = shdTexParams[texIndex];
			auto& param = shdParamMap[src.paramIndex];

			texParam._tex = src.defaultTex;
			texParam._bindPoint = param.bindPoint;
			texParam._stageFlags = param.stageFlags;
			texIndex++;
		}
	}

	{ // storage params;
		auto& shdStorageBufParams = newShaderPass->storageBufParams();
		auto& shdParamMap  = newShaderPass->paramMap();

		axInt texIndex = 0;
		_storageBufParams.resize(shdStorageBufParams.size());
		for (auto& bufParam : _storageBufParams) {
			auto& src   = shdStorageBufParams[texIndex];
			auto& param = shdParamMap[src.paramIndex];

			bufParam._rawUAV = src.rawUAV;
			bufParam._bindPoint = param.bindPoint;
			bufParam._stageFlags = param.stageFlags;
			texIndex++;
		}
	}

	onSetShaderPass(newShaderPass);
}

template<class T>
void axMaterialPass::_setParam(axNameId name, const T& v) {
	if (!_shaderPass) return;

	auto* param = _shaderPass->findParam(name);
	if (!param) return;

	if (param->dataType != DataTypeUtil::typeOf<T>()) throw axError_Undefined(AX_LOC);
	if (param->dataRange.size != ax_sizeof(v)) throw axError_Undefined(AX_LOC);

	auto* uniformBuffer = _uniformBuffers.tryAt(param->bufferIndex);
	if (!uniformBuffer) throw axError_Undefined(AX_LOC);

	auto dst = uniformBuffer->_paramValues.slice(param->dataRange);
	auto& dstValue = *reinterpret_cast<T*>(dst.data());

	if (dstValue == v) return;
	dstValue = v;
	
	uniformBuffer->_dirtyRange |= param->dataRange;
}

void axMaterialPass::setParam(axNameId name, const float&			v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const double&			v) { _setParam(name, v); }

void axMaterialPass::setParam(axNameId name, const axInt32&			v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axInt32x2&		v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axInt32x3&		v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axInt32x4&		v) { _setParam(name, v); }

void axMaterialPass::setParam(axNameId name, const axUInt32&		v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axUInt32x2&		v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axUInt32x3&		v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axUInt32x4&		v) { _setParam(name, v); }

void axMaterialPass::setParam(axNameId name, const axVec2f&			v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axVec3f&			v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axVec4f&			v) { _setParam(name, v); }
void axMaterialPass::setParam(axNameId name, const axMat4f&			v) { _setParam(name, v.transpose()); }
void axMaterialPass::setParam(axNameId name, const axColorRGBAf&	v) { _setParam(name, axVec4f_make(v)); }
void axMaterialPass::setParam(axNameId name, const axColorRGBf&		v) { _setParam(name, axVec3f_make(v)); }
void axMaterialPass::setParam(axNameId name,       Texture2D*		v) { _setTexture(name, v); }

void axMaterialPass::setParam(axNameId name,       StorageBuffer*	v) { _setStorageBuffer(name, v); }

void axMaterialPass::onUpdateGpuResource(axRenderRequest& req) {
	for (auto& cb : _uniformBuffers) {
		cb.onUpdateGpuResource(req);
	}
}

template<class T>
void axMaterialPass::_setTexture(axNameId name, T* texture) {
	if (!texture) throw axError_Undefined(AX_LOC);

	if (!_shaderPass) return;

	auto* param = _shaderPass->findParam(name);
	if (!param) return;
	if (param->dataType != DataTypeUtil::typeOf<T>()) throw axError_Undefined(AX_LOC);

	auto* texParam = _texParams.tryAt(param->bufferIndex);
	if (!texParam) throw axError_Undefined(AX_LOC);

	texParam->_tex.ref(texture);
}

void axMaterialPass::_setStorageBuffer(axNameId name, StorageBuffer* buffer) {
	if (!buffer) throw axError_Undefined(AX_LOC);

	if (!_shaderPass) return;
	auto* param = _shaderPass->findParam(name);
	if (!param) return;

	auto* bufParam = _storageBufParams.tryAt(param->bufferIndex);
	if (!bufParam) throw axError_Undefined(AX_LOC);

	bufParam->_storageBuffer.ref(buffer);
}

void axMaterialPass::UniformBuffer::onUpdateGpuResource(axRenderRequest &req) {
	if (!_multiGpuBuffer) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Const;
		_multiGpuBuffer = req.renderer()->createMultiGpuRenderBuffer(desc);
		_dirtyRange.set(0, dataSize());
	}

	if (_dirtyRange.size) {
		_multiGpuBuffer->syncData(req, _paramValues, _dirtyRange);
	}
	_dirtyRange.set(0,0);
}

axTexture* axMaterialPass::TexParam::getUpdatedTexture() {
	if (!_tex) {
		auto* assets = axRenderer::s_instance()->builtInAssets();
		switch (_dataType) {
			case DataType::Texture1D: _tex = assets->dummyMissingTex1D; break;
			case DataType::Texture2D: _tex = assets->dummyMissingTex2D; break;
			case DataType::Texture3D: _tex = assets->dummyMissingTex3D; break;
			default: throw axError_Undefined(AX_LOC);
		}
	}

	// TODO update texture
	return _tex;
}
