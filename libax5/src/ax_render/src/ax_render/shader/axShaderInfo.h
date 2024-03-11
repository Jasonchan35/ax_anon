#pragma once

#include "axRenderState.h"

#define axShaderPropType_EnumList(E) \
	E(Unknown,) \
	\
	E(Int8,  )   E(Int8x2,)    E(Int8x3,)    E(Int8x4,) \
	E(Int16, )   E(Int16x2,)   E(Int16x3,)   E(Int16x4,) \
	E(Int32, )   E(Int32x2,)   E(Int32x3,)   E(Int32x4,) \
	E(Int64, )   E(Int64x2,)   E(Int64x3,)   E(Int64x4,) \
	\
	E(UInt8,  )  E(UInt8x2,)   E(UInt8x3,)   E(UInt8x4,) \
	E(UInt16, )  E(UInt16x2,)  E(UInt16x3,)  E(UInt16x4,) \
	E(UInt32, )  E(UInt32x2,)  E(UInt32x3,)  E(UInt32x4,) \
	E(UInt64, )  E(UInt64x2,)  E(UInt64x3,)  E(UInt64x4,) \
	\
	E(Float16, ) E(Float16x2,) E(Float16x3,) E(Float16x4,) \
	E(Float32, ) E(Float32x2,) E(Float32x3,) E(Float32x4,) \
	E(Float64, ) E(Float64x2,) E(Float64x3,) E(Float64x4,) \
	\
	E(Float16_2x2, ) E(Float16_3x3,) E(Float16_4x4,) E(Float16_4x3,) \
	E(Float32_2x2, ) E(Float32_3x3,) E(Float32_4x4,) E(Float32_4x3,) \
	E(Float64_2x2, ) E(Float64_3x3,) E(Float64_4x4,) E(Float64_4x3,) \
	\
	E(Bool,)  E(Boolx2,)  E(Boolx3,)  E(Boolx4,) \
	\
	E(Texture2D,) \
	E(Color,) \
	E(Range,) \
//----
ax_ENUM_CLASS(axShaderPropType, axUInt16)

inline
axRenderDataType axShaderPropTypeToDataType(axShaderPropType t) {
	using PropType = axShaderPropType;
	using DataType = axRenderDataType;
	switch (t) {
		case PropType::Unknown:		return DataType::Unknown;
	//---
		case PropType::Int8:		return DataType::Int8;
		case PropType::Int8x2:		return DataType::Int8x2;
		case PropType::Int8x3:		return DataType::Int8x3;
		case PropType::Int8x4:		return DataType::Int8x4;
	//---
		case PropType::Int16:		return DataType::Int16;
		case PropType::Int16x2:		return DataType::Int16x2;
		case PropType::Int16x3:		return DataType::Int16x3;
		case PropType::Int16x4:		return DataType::Int16x4;
	//---
		case PropType::Int32:		return DataType::Int32;
		case PropType::Int32x2:		return DataType::Int32x2;
		case PropType::Int32x3:		return DataType::Int32x3;
		case PropType::Int32x4:		return DataType::Int32x4;
	//---
		case PropType::Int64:		return DataType::Int64;
		case PropType::Int64x2:		return DataType::Int64x2;
		case PropType::Int64x3:		return DataType::Int64x3;
		case PropType::Int64x4:		return DataType::Int64x4;
	//---
		case PropType::Float16:		return DataType::Float16;
		case PropType::Float16x2:	return DataType::Float16x2;
		case PropType::Float16x3:	return DataType::Float16x3;
		case PropType::Float16x4:	return DataType::Float16x4;
	//---
		case PropType::Float32:		return DataType::Float32;
		case PropType::Float32x2:	return DataType::Float32x2;
		case PropType::Float32x3:	return DataType::Float32x3;
		case PropType::Float32x4:	return DataType::Float32x4;
	//---
		case PropType::Float64:		return DataType::Float64;
		case PropType::Float64x2:	return DataType::Float64x2;
		case PropType::Float64x3:	return DataType::Float64x3;
		case PropType::Float64x4:	return DataType::Float64x4;
	//---
		case PropType::Texture2D:	return DataType::Texture2D;
		default:
			throw axError_Undefined(AX_LOC);
	}
}

class axShaderVertexAttrIdHelper {
public:
	axShaderVertexAttrIdHelper();

	using AttrId = axRenderVertexAttrId;
	static AttrId fromVarName	(axStrView name);

private:
	static axShaderVertexAttrIdHelper* s_instance();

	axDict<axString, AttrId>	_nameToAttrIdDict;
};

struct axShaderStageProfile {
	static const axStrLiteral DX11_VS;
	static const axStrLiteral DX11_PS;
	static const axStrLiteral DX11_CS;

	static const axStrLiteral DX12_VS;
	static const axStrLiteral DX12_PS;
	static const axStrLiteral DX12_CS;
	static const axStrLiteral DX12_Lib;

	static const axStrLiteral GLSL_VS;
	static const axStrLiteral GLSL_PS;
	static const axStrLiteral GLSL_CS;

	static const axStrLiteral GLSL_ES_VS;
	static const axStrLiteral GLSL_ES_PS;
	static const axStrLiteral GLSL_ES_CS;

	static const axStrLiteral MSL_VS_Mac;
	static const axStrLiteral MSL_PS_Mac;
	static const axStrLiteral MSL_CS_Mac;
	static const axStrLiteral MSL_Lib_Mac;

	static const axStrLiteral MSL_VS_iOS;
	static const axStrLiteral MSL_PS_iOS;
	static const axStrLiteral MSL_CS_iOS;
	static const axStrLiteral MSL_Lib_iOS;

	static const axStrLiteral SPIRV_VS;
	static const axStrLiteral SPIRV_PS;
	static const axStrLiteral SPIRV_CS;

	static const axStrLiteral ISPC_VS;
	static const axStrLiteral ISPC_PS;
	static const axStrLiteral ISPC_CS;
};

class axShaderStageInfo {
public:
	using DataType		= axRenderDataType;
	using VertexAttrId	= axRenderVertexAttrId;
	using StageFlags	= axShaderStageFlags;

	void loadFile(axStrView filename_) {
		filename = filename_;
		axJsonUtil::readFile(filename_, *this);
	}

	axString	filename;
	axString	profile;
	StageFlags	stageFlags = StageFlags::Unknown;

	axVec3i		csWorkgroupSize {1};

	class Param {
	public:
		axString name;
		DataType dataType;
		axInt16	bindPoint = 0;
		axInt16	bindCount = 0;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, dataType);
			AX_SE_NAMED_IO(se, bindPoint);
			AX_SE_NAMED_IO(se, bindCount);
		}
	};

	class Input {
	public:
		axString		name;
		VertexAttrId	attrId   = VertexAttrId::Unknown;
		DataType		dataType = DataType::Unknown;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, attrId);
			AX_SE_NAMED_IO(se, dataType);
		}
	};

	class Variable {
	public:
		axString	name;
		axInt		offset   = 0;
		DataType	dataType = DataType::Unknown;
		bool		rowMajor = true;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, offset);
			AX_SE_NAMED_IO(se, dataType);
		}
	};

	class UniformBuffer {
	public:
		axString			name;
		axInt16				bindPoint = 0;
		axInt16				bindCount = 0;
		axInt				dataSize  = 0;
		axArray<Variable>	variables;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, bindPoint);
			AX_SE_NAMED_IO(se, bindCount);
			AX_SE_NAMED_IO(se, dataSize);
			AX_SE_NAMED_IO(se, variables);
		}
	};

	axArray<Input>			inputs;
	axArray<Param>			params;
	axArray<UniformBuffer>	uniformBuffers;

//----------
	class Texture {
	public:
		axString	name;
		axInt16		bindPoint = 0;
		axInt16		bindCount = 0;
		DataType	dataType = DataType::Unknown;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, bindPoint);
			AX_SE_NAMED_IO(se, bindCount);
			AX_SE_NAMED_IO(se, dataType);
		}
	};
	axArray<Texture>		textures;

//----------
	class Sampler {
	public:
		axString	name;
		axInt16		bindPoint = 0;
		axInt16		bindCount = 0;
		DataType	dataType = DataType::Unknown;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, bindPoint);
			AX_SE_NAMED_IO(se, bindCount);
			AX_SE_NAMED_IO(se, dataType);
		}
	};
	axArray<Sampler>		samplers;

//----------
	class StorageBuffer {
	public:
		axString	name;
		axInt16		bindPoint = 0;
		axInt16		bindCount = 0;
		bool		rawUAV = false;

		template<class SE>
		void onSerialize(SE & se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, bindPoint);
			AX_SE_NAMED_IO(se, bindCount);
			AX_SE_NAMED_IO(se, rawUAV);
		}
	};
	axArray<StorageBuffer>	storageBuffers;


//----------
	template<class SE>
	void onSerialize(SE & se) {
		AX_SE_NAMED_IO(se, profile);
		AX_SE_NAMED_IO(se, csWorkgroupSize);
		AX_SE_NAMED_IO(se, inputs);
		AX_SE_NAMED_IO(se, params);
		AX_SE_NAMED_IO(se, uniformBuffers);
		AX_SE_NAMED_IO(se, textures);
		AX_SE_NAMED_IO(se, samplers);
		AX_SE_NAMED_IO(se, storageBuffers);
	}
};

class axShaderStageInfoSet {
public:
	axShaderStageInfoSet();

	axShaderStageInfo	vsInfo;
	axShaderStageInfo	psInfo;
	axShaderStageInfo	csInfo;
};

struct axShaderPassInfo {
	axString		name;
	axString		vsFunc;
	axString		psFunc;
	axString		csFunc;
	axString		rayTracingFunc;
	axRenderState	renderState;

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_NAMED_IO(se, name);
		AX_SE_NAMED_IO(se, vsFunc);
		AX_SE_NAMED_IO(se, psFunc);
		AX_SE_NAMED_IO(se, csFunc);
		AX_SE_NAMED_IO(se, renderState);
	}
};

class axShaderInfo {
public:
	using PropType		= axShaderPropType;
	using Pass			= axShaderPassInfo;

	struct Prop {
		PropType	propType;
		axString	name;
		axString	displayName;
		axString	defaultValue;
		template<class SE>
		void onSerialize(SE& se) {
			AX_SE_NAMED_IO(se, name);
			AX_SE_NAMED_IO(se, propType);
			AX_SE_NAMED_IO(se, displayName);
			AX_SE_NAMED_IO(se, defaultValue);
		}
	};
	axArray<Prop>			props;
	axArray<Pass>			passes;

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_NAMED_IO(se, props);
		AX_SE_NAMED_IO(se, passes);
	}
};
