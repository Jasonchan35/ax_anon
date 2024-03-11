#pragma once

#include "../vertex/axRenderVertexDesc.h"
#include "../texture/axTexture.h"
#include "../axRenderResource.h"
#include "axRenderState.h"
#include "axShaderInfo.h"

class axShader;

class axShaderPass : public axRenderResource {
	AX_RTTI_CLASS(axShaderPass, axRenderResource)
public:
	using Info			= axShaderPassInfo;
	using StageProfile	= axShaderStageProfile;
	using StageInfo		= axShaderStageInfo;
	using StageInfoSet	= axShaderStageInfoSet;
	using StageFlags	= axShaderStageFlags;

			bool ready	();	
	virtual bool onValidate	() = 0;

	void onZeroRefCount() {
		//queue up to destroy
	}
	

//------
	class VertexAttr : public axNonCopyable {
	public:
		axInt			bindPoint	= 0; // opengl loc
		VertexAttrId	attrId		= VertexAttrId::Unknown;
		DataType		dataType	= DataType::Unknown;
	};

	const VertexAttr* findVertexAttr(VertexAttrId attrId) const { return ax_const_cast(this)->_findMutVertexAttr(attrId); }
	void checkAcceptVertexDesc(VertexDesc vertexDesc) const;

//---------
	axShader*	shader() { return _shader; }
	axInt		passIndex() const { return _passIndex; }

	const axRenderState&	staticRenderState() const { return _staticRenderState; }

	struct UniformBuffer : public axNonCopyable {
		axInt	index		= 0;
		axInt	bindPoint	= 0;
		axInt	_glLoc		= 0;
		axByteArray	defaultParamValues;
		StageFlags	stageFlags	= StageFlags::Unknown;
	};

	struct Param : public axNonCopyable {
		axInt		index		= 0;
		axIntRange	dataRange	= {};
		axInt		bufferIndex	= 0; // uniformBufferIndex or texture index or storageBuffer index
		DataType	dataType	= DataType::Unknown;
		axInt		bindPoint	= 0;
		StageFlags	stageFlags	= StageFlags::Unknown;

		void onFormat(axFormat& f) const { f.format("index={?}, dataType={?} bindPoint={?}", index, dataType, bindPoint); }
	//----
//		axInt		texResourceIndex = 0;
	};
	Param* findParam(axNameId  name );

	struct TexParam {
		axInt		paramIndex = 0;
		axSPtr<Texture>	defaultTex;
	};

	struct StorageBufferParam {
		axInt	paramIndex = 0;
		bool	rawUAV = false;
	};

	struct SamplerState {
		axInt index = 0;
		axInt bindPoint = 0;
	};

	void init(axShader* shader, axInt passIndex, const Info& info);

	using VertexAttrs			= axArray<VertexAttr>;
	using TexParams				= axArray<TexParam, 16>;
	using StorageBufferParams	= axArray<StorageBufferParam, 8>;
	using SamplerStateMap		= axArrayMap<axInt, SamplerState>; // key = bindPoint
	using UniformBufferMap		= axArrayMap<axInt, UniformBuffer>;  // key = bindPoint
	using ParamMap				= axArrayMap<axNameId, Param>;

	const UniformBufferMap&		uniformBufferMap	() const	{ return _uniformBufferMap; }
	const ParamMap&				paramMap			() const	{ return _paramMap; }
	const SamplerStateMap&		samplerStateMap		() const	{ return _samplerStateMap; }
	const TexParams&			texParams			() const	{ return _texParams; }
	const StorageBufferParams&	storageBufParams	() const	{ return _storageBufParams; }

	static void validateVertexAttr(const VertexAttr& reqAttr, const axRenderVertexDesc::Attr& attr);

	bool isCompute() const { return ax_enum(_stageFlags).hasFlags(StageFlags::Compute); }

	StageFlags stageFlags() const { return _stageFlags; }

protected:
	axShaderPass();

			void initStageInfos(const Info& info, axStrView vsInfoFilename, axStrView psInfoFilename, axStrView csInfoFilename);
	virtual void onInitStageInfos(StageInfoSet& infoSet) {}

			void initStageInfo(StageInfo& stageInfo);

	VertexAttr&		_addVertexAttr	(VertexAttrId attrId, DataType dataType, axInt bindPoint);
	VertexAttr* 	_findMutVertexAttr(VertexAttrId attrId);

	UniformBuffer&	_addUniformBuffer		(const StageFlags& stageFlags, axNameId name, axInt dataSize, axInt bindPoint, axInt glLoc);
	Param&			_addUniformParam		(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt uniformBuffer, axInt dataOffset);
	Param&			_addTextureParam		(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt bindPoint);
	SamplerState&	_addSamplerState		(const StageFlags& stageFlags, axNameId name, axInt bindPoint);
	Param&			_addStorageBufferParam	(const StageFlags& stageFlags, axNameId name, axInt bindPoint, bool rawUAV);

	Param&			_internalAddParam		(const StageFlags& stageFlags, axNameId name, DataType dataType, axInt uniformBuffer, axInt dataOffset, axInt bindPoint, bool rawUAV);

	virtual	void onInit(axInt passIndex, const Info& info) = 0;

	enum class State {
		Unknown,
		Loaded,
		Compiled,
		Ready,
		Error,
	};

	axShader*			_shader = nullptr;
	axInt				_passIndex = -1;

	State				_state {State::Unknown};
	axNameId			_name;
	axString			_vsFuncName;
	axString			_psFuncName;
	axString			_csFuncName;

	VertexAttrs			_vertexAttrs;
	VertexAttrMask		_vertexAttrMask = VertexAttrMask::None;
	UniformBufferMap	_uniformBufferMap;
	ParamMap			_paramMap;
	TexParams			_texParams;
	StorageBufferParams	_storageBufParams;
	SamplerStateMap		_samplerStateMap;
	StageFlags			_stageFlags	= StageFlags::Unknown;
	axRenderState		_staticRenderState;
	bool				_paramNotFoundWarning : 1;
};
