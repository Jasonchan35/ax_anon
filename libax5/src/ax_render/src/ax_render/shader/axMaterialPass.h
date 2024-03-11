#pragma once

#include "axShaderPass.h"
#include "../buffer/axRenderMultiGpuBuffer.h"

class axMaterialPass : public axRenderResource {
public:
	using StageFlags = axShaderStageFlags;

	struct UniformBuffer : public axNonCopyable {
		axInt	dataSize() { return _paramValues.size(); }
		axInt	bindPoint() const { return _bindPoint; }
		axInt	glLoc() const { return _glLoc; }
		const StageFlags& stageFlags() const { return _stageFlags; }

		axRenderGpuBuffer* currentGpuBuffer() { return _multiGpuBuffer ? _multiGpuBuffer->currentGpuBuffer() : nullptr; }

	friend class axMaterialPass;
	protected:
		axInt	_bindPoint	= 0;
		axInt	_glLoc = 0;

		axByteArray		_paramValues;
		axIntRange		_dirtyRange = {};
		StageFlags		_stageFlags = StageFlags::Unknown;
		axSPtr<MultiGpuBuffer>	_multiGpuBuffer;

		void onUpdateGpuResource(axRenderRequest& req);
	};

	struct TexParam : public axNonCopyable {
				Texture*	getUpdatedTexture();
				axInt		bindPoint	() const { return _bindPoint; }
		const StageFlags&	stageFlags	() const { return _stageFlags; }

	friend class axMaterialPass;
	protected:
		axSPtr<Texture>	_tex;
		axInt		_bindPoint = 0;
		DataType	_dataType = DataType::Unknown;
//		bool		_updated = false;
		StageFlags	_stageFlags = StageFlags::Unknown;
	};

	struct StorageBufferParam : public axNonCopyable {
				axInt			bindPoint		() const	{ return _bindPoint; }
				StorageBuffer*	storageBuffer	()			{ return _storageBuffer; }
		const StageFlags&		stageFlags		() const	{ return _stageFlags; }
						bool	rawUAV			() const	{ return _rawUAV; }

	friend class axMaterialPass;
	protected:
		axInt	_bindPoint = 0;
		axSPtr<StorageBuffer> _storageBuffer;
		StageFlags	_stageFlags = StageFlags::Unknown;
		bool _rawUAV = false;
	};

	void setShaderPass(axShaderPass* newShaderPass);

	axShaderPass*		shaderPass()	{ return _shaderPass; }
	axSpan<TexParam>	texParams()		{ return _texParams; }

	void setParam	(axNameId name, const float&   v);
	void setParam	(axNameId name, const double&  v);

	void setParam	(axNameId name, const axInt32&   v);
	void setParam	(axNameId name, const axInt32x2& v);
	void setParam	(axNameId name, const axInt32x3& v);
	void setParam	(axNameId name, const axInt32x4& v);

	void setParam	(axNameId name, const axUInt32&   v);
	void setParam	(axNameId name, const axUInt32x2& v);
	void setParam	(axNameId name, const axUInt32x3& v);
	void setParam	(axNameId name, const axUInt32x4& v);

	void setParam	(axNameId name, const axVec2f& v);
	void setParam	(axNameId name, const axVec3f& v);
	void setParam	(axNameId name, const axVec4f& v);

	void setParam	(axNameId name, const axMat4f& v);

	void setParam	(axNameId name, const axColorRGBAf& v);
	void setParam	(axNameId name, const axColorRGBf&  v);
	void setParam	(axNameId name, Texture2D* tex);
	void setParam	(axNameId name, StorageBuffer* buf);

	void onUpdateGpuResource(axRenderRequest& req);
protected:
	axMaterialPass() = default;

	virtual void onSetShaderPass(axShaderPass* newShaderPass) {}

	template<class T>
	void _setParam	(axNameId name, const T& v);

	template<class T>
	void _setTexture(axNameId name, T* texture);

	void _setStorageBuffer(axNameId name, StorageBuffer* buffer);

	axSPtr<axShaderPass>			_shaderPass;
	axArray<TexParam, 8>			_texParams;
	axArray<UniformBuffer, 8>		_uniformBuffers;
	axArray<StorageBufferParam, 8>	_storageBufParams;
};
