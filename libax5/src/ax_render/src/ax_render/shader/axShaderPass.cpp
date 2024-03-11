#include "axShaderPass.h"
#include "axShader.h"
#include "../axRenderer.h"

axShaderPass::axShaderPass() 
	: _paramNotFoundWarning(true)
//	, _vertexDesc(&_vertexDescData)
{
}

bool axShaderPass::ready() {
	switch (_state) {
		case State::Ready: return true;
		case State::Error: return false;
		
		case State::Compiled: {
			if (!onValidate()) return false;
			_state = State::Ready;
			return true;
		}break;
			
		default: return false;
	}
}

axShaderPass::VertexAttr* axShaderPass::_findMutVertexAttr(VertexAttrId attrId) {
	for (auto& a : _vertexAttrs) {
		if (a.attrId == attrId) return &a;
	}
	return nullptr;
}

axShaderPass::Param* axShaderPass::findParam(axNameId name) {
	auto* p = _paramMap.find(name);
	if (!p && _paramNotFoundWarning) {
		AX_LOG_WARNING("param[{?}] not found in shader {?}", name, _shader->filename());
		_paramNotFoundWarning = false;
	}
	return p;
}

void axShaderPass::init(axShader* shader, axInt passIndex, const Info& info) {
	_state = State::Compiled;

	_shader = shader;
	_passIndex = passIndex;

	_name = axNameId::s_make(info.name);

	if (info.vsFunc) _stageFlags |= axShaderStageFlags::Vertex;
	if (info.psFunc) _stageFlags |= axShaderStageFlags::Pixel;
	if (info.csFunc) _stageFlags |= axShaderStageFlags::Compute;


	_vsFuncName = info.vsFunc;
	_psFuncName = info.psFunc;
	_csFuncName = info.csFunc;
	_staticRenderState = info.renderState;

	if (info.csFunc) {
		auto* renderer = axRenderer::s_instance();
		if (!renderer->adapterInfo().computeShader) {
			return;
		}
	}

	onInit(passIndex, info);

//----
	AX_ASSERT(_csFuncName || _vertexAttrs.size() > 0);
}

axShaderPass::VertexAttr& axShaderPass::_addVertexAttr(VertexAttrId attrId, DataType dataType, axInt bindPoint) {
	if (attrId		== VertexAttrId::Unknown) throw axError_Undefined(AX_LOC);
	if (dataType	== DataType::Unknown) throw axError_Undefined(AX_LOC);

	auto attrMask = VertexAttrMask(1) << ax_enum_int(attrId);

	if ((_vertexAttrMask & attrMask) != VertexAttrMask::None) {
		throw axError_Undefined(AX_LOC); //already exists;
	}

	_vertexAttrMask |= attrMask;

	auto& a = _vertexAttrs.emplaceBack();
	a.attrId = attrId;
	a.dataType = dataType;
	a.bindPoint = bindPoint;

	return a;
}

axShaderPass::UniformBuffer& axShaderPass::_addUniformBuffer(const StageFlags& stageFlags, axNameId name, axInt dataSize, axInt bindPoint, axInt glLoc) {
	const auto& key = bindPoint;
	
	auto* p = _uniformBufferMap.find(key);
	if (p) {
		if (p->defaultParamValues.size() != dataSize )
			throw axError_Undefined(AX_LOC);
		if (p->bindPoint != bindPoint)	throw axError_Undefined(AX_LOC);
		if (p->_glLoc    != glLoc)		throw axError_Undefined(AX_LOC);

	}else{
		auto index = _uniformBufferMap.size();
		p = &_uniformBufferMap.add(key);

		p->index		= index;
		p->bindPoint	= bindPoint;
		p->_glLoc		= glLoc;
		p->defaultParamValues.resize(dataSize);
		ax_bzero(p->defaultParamValues);
	}

	ax_enum(p->stageFlags).setFlags(stageFlags);
	return *p;
}

axShaderPass::Param&		axShaderPass::_addUniformParam		(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt uniformBuffer, axInt dataOffset) {
	return _internalAddParam(stageFlags, name, dataType, uniformBuffer, dataOffset, -1, false);
}

axShaderPass::Param&		axShaderPass::_addTextureParam		(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt bindPoint) {
	return _internalAddParam(stageFlags, name, dataType, -1, 0, bindPoint, false);
}

axShaderPass::Param&		axShaderPass::_addStorageBufferParam(const StageFlags& stageFlags, axNameId name, axInt bindPoint, bool rawUAV) {
	return _internalAddParam(stageFlags, name, DataType::StorageBuffer, -1, 0, bindPoint, rawUAV);
}

axShaderPass::Param& axShaderPass::_internalAddParam(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt bufferIndex, axInt dataOffset, axInt bindPoint, bool rawUAV) {
	const auto& key = name;

	auto* p = _paramMap.find(key);
	if (p) {
		if (p->dataType			!= dataType)	throw axError_Undefined(AX_LOC);
		if (p->bufferIndex		!= bufferIndex)	throw axError_Undefined(AX_LOC);
		if (p->dataRange.start	!= dataOffset)	throw axError_Undefined(AX_LOC);
		if (p->bindPoint		!= bindPoint)	throw axError_Undefined(AX_LOC);

	} else {
		auto index = _paramMap.size();
		p = &_paramMap.add(key);

		p->index = index;
		p->bufferIndex = bufferIndex;
		p->dataType = dataType;
		p->bindPoint = bindPoint;
		p->dataRange.set(dataOffset, DataTypeUtil::sizeInBytes(dataType));

		switch (dataType) {
			case DataType::Texture1D:
			case DataType::Texture2D:
			case DataType::Texture3D: 
			case DataType::TextureCube: {
				p->bufferIndex = _texParams.size();
				auto& s = _texParams.emplaceBack();
				s.paramIndex = index;
			}break;

			case DataType::StorageBuffer: {
				p->bufferIndex = _storageBufParams.size();
				auto& s = _storageBufParams.emplaceBack();
				s.paramIndex = index;
				s.rawUAV = rawUAV;
			}break;
		}
	}

	ax_enum(p->stageFlags).setFlags(stageFlags);
	return *p;
}

axShaderPass::SamplerState& axShaderPass::_addSamplerState(const StageFlags& stageFlags, axNameId name, axInt bindPoint) {
	const auto& key = bindPoint;

	auto* p = _samplerStateMap.find(key);
	if (p) {
		// TODO check values
	} else {
		auto index = _samplerStateMap.size();
		p = &_samplerStateMap.add(key);
		p->index = index;
		p->bindPoint = bindPoint;
	}

	return *p;
}

void axShaderPass::checkAcceptVertexDesc(VertexDesc vertexDesc) const {
	if (vertexDesc == nullptr) {
		throw axError_Undefined(AX_LOC);
	}

	for (auto& va : _vertexAttrs) {
		auto ra = vertexDesc->attr(va.attrId);
		validateVertexAttr(va, ra);
	}
}

void axShaderPass::validateVertexAttr(const VertexAttr& reqAttr, const axRenderVertexDesc::Attr& attr) {
	if (!attr.active) {
		AX_LOG_WARNING("Mesh missing vertex attr {?} required by shader", reqAttr.attrId);
		throw axError_Undefined(AX_LOC);
	}

	if (reqAttr.attrId == VertexAttrId::POSITION0) {
		// for pos, it's float3 in CPU side, but float4 in shader
	} else {
		if (DataTypeUtil::elementCount(reqAttr.dataType) != DataTypeUtil::elementCount(attr.dataType)) {
			AX_LOG_WARNING("Mesh vertex attr {?} element count mismatch require={?} input={?}", reqAttr.attrId, reqAttr.dataType, attr.dataType);
			throw axError_Undefined(AX_LOC);
		}
	}
}

void axShaderPass::initStageInfos(const Info& info, axStrView vsInfoFilename, axStrView psInfoFilename, axStrView csInfoFilename) {
	axShaderStageInfoSet infoSet;
	
	if (info.vsFunc) infoSet.vsInfo.loadFile(vsInfoFilename);
	if (info.psFunc) infoSet.psInfo.loadFile(psInfoFilename);
	if (info.csFunc) infoSet.csInfo.loadFile(csInfoFilename);

	auto paramMaxCount	= infoSet.vsInfo.params.size() 
						+ infoSet.psInfo.params.size()
						+ infoSet.csInfo.params.size();
	_paramMap.reserve(paramMaxCount);

	auto uniformBufferMaxCount	= infoSet.vsInfo.uniformBuffers.size()
								+ infoSet.psInfo.uniformBuffers.size()
								+ infoSet.csInfo.uniformBuffers.size();
	_uniformBufferMap.reserve(uniformBufferMaxCount);

	{
		_vertexAttrs.reserve(infoSet.vsInfo.inputs.size());
		for (auto& vi : infoSet.vsInfo.inputs) {
			_addVertexAttr(vi.attrId, vi.dataType, 0);
		}
	}

	initStageInfo(infoSet.vsInfo);
	initStageInfo(infoSet.psInfo);
	initStageInfo(infoSet.csInfo);

	onInitStageInfos(infoSet);
}

void axShaderPass::initStageInfo(StageInfo& stageInfo) {
	auto& stage = stageInfo.stageFlags;

	for (auto& src : stageInfo.uniformBuffers) {
		auto& cb = _addUniformBuffer(stage, axNameId::s_make(src.name), src.dataSize, src.bindPoint, 0);
		for (auto& varInfo : src.variables) {
			_addUniformParam(stage, axNameId::s_make(varInfo.name), varInfo.dataType, cb.index, varInfo.offset);
		}
	}

	{
		axInt texIndex = 0;
		for (auto& src : stageInfo.textures) {
			_addTextureParam(stage, axNameId::s_make(src.name), src.dataType, src.bindPoint);
			texIndex++;
		}
	}

	{
		axInt sbufIndex = 0;
		for (auto& src : stageInfo.storageBuffers) {
			_addStorageBufferParam(stage, axNameId::s_make(src.name), src.bindPoint, src.rawUAV);
			sbufIndex++;
		}
	}

	for (auto& src : stageInfo.samplers) {
		_addSamplerState(stage, axNameId::s_make(src.name), src.bindPoint);
	}
}
