#pragma once

#include "axRenderDataType.h"
#include "vertex/axRenderVertex.h"

class axRenderGpuBuffer;

class axMaterial;
class axMaterialPass;

class axShader;
class axShaderPass;

class axTexture;
class axTexture1D;
class axTexture2D;
class axTexture2D_UploadRequest;
class axTexture2D_CreateDesc;
class axTexture3D;
class axTexture3D_CreateDesc;
class axTextureCube;
class axTextureSprite;

class axRenderer;
class axRenderBuiltInAssets;
class axRenderComputeCall;
class axRenderContext;
class axRenderContext_EventHandler;
class axRenderContext_CreateDesc;

class axRenderDataTypeUtil;
class axRenderDrawCall;

class axRenderFence;

class axRenderGpuQuery;
class axRenderGpuQuery_CreateDesc;
class axRenderGpuBuffer;
class axRenderGpuBuffer_CreateDesc;
class axRenderCommandDispatcher;
class axRenderMesh;
class axRenderMultiGpuBuffer;
class axRenderRequest;

class axRenderStageBuffer;
class axRenderStageBuffer_CreateDesc;
class axRenderStorageBuffer;
class axRenderStorageBuffer_CreateDesc;

class axRenderState;
class axRenderSubMesh;

enum class axRenderVertexAttrId : axUInt8;
enum class axRenderIndexType 	: axUInt8;
enum class axRenderVertexType 	: axUInt64;

class axRenderVertexArray;
class axRenderVertexDesc;
class axRenderIndexArray;

class axSamplerState;
class axShader;

class axMaterial;
class axMaterialPass;

class axRenderCommonBase {
public:
	using BlendOp			= axRenderState_BlendOp;
	using BlendFactor		= axRenderState_BlendFactor;
	using BuiltInAssets		= axRenderBuiltInAssets;

	using ColorType			= axColorType;
	using ComputeCall		= axRenderComputeCall;

	using DataType			= axRenderDataType;
	using DataTypeUtil		= axRenderDataTypeUtil;
	using DepthTestOp		= axRenderState_DepthTestOp;
	using DrawCall			= axRenderDrawCall;

	using Fence				= axRenderFence;

	using GpuBuffer			= axRenderGpuBuffer;
	using GpuQuery			= axRenderGpuQuery;

	using IndexArray		= axRenderIndexArray;
	using IndexType			= axRenderIndexType;

	using PrimitiveType 	= axRenderPrimitiveType;

	using Rect2				= axRect2f;
	using RenderContext		= axRenderContext;
	using RenderState		= axRenderState;

	using Margin2			= axMargin2f;
	using Mat4				= axMat4f;
	using Material			= axMaterial;
	using MaterialPass		= axMaterialPass;
	using Mesh				= axRenderMesh;
	using MultiGpuBuffer	= axRenderMultiGpuBuffer;

	using SamplerState		= axSamplerState;

	using Shader			= axShader;
	using ShaderPass		= axShaderPass;
	using ShaderStageFlags	= axShaderStageFlags;
	using StageBuffer		= axRenderStageBuffer;
	using StorageBuffer		= axRenderStorageBuffer;
	using SubMesh			= axRenderSubMesh;
	using Sprite			= axTextureSprite;

	using Texture			= axTexture;
	using Texture1D			= axTexture1D;
	using Texture2D			= axTexture2D;
	using Texture3D			= axTexture3D;
	using TextureCube		= axTextureCube;

	using Vec2				= axVec2f;
	using Vec3				= axVec3f;
	using Vec4				= axVec4f;
	using VertexArray		= axRenderVertexArray;
	using VertexAttrId		= axRenderVertexAttrId;
	using VertexAttrMask	= axRenderVertexAttrMask;
	using VertexDesc		= axRenderVertexDesc;
	using VertexType		= axRenderVertexType;
};

struct axRenderCommonBase_Dummy : public axRenderCommonBase {
	axByte foo;
};

AX_STATIC_ASSERT(sizeof(axRenderCommonBase_Dummy) == 1); // ensure no member in axRenderCommonBase

struct axRenderMemoryInfo {
	axInt used = 0;
	axInt budget = 0;
};

struct axRenderNames {
	#define E(X) \
		static axNameId X() { static axNameId _s = axNameId::s_make(#X); return _s; } \
	//----
		E(ambient)
		E(cameraPos)
		E(color)
		E(color0)
		E(color1)
		E(color2)
		E(color3)
		E(diffuse)
		E(glyphTex)
		E(lightColor)
		E(lightPos)
		E(lightPower)
		E(mainTex0)
		E(mainTex1)
		E(mainTex2)
		E(mainTex3)
		E(mainTex4)
		E(mainTex5)
		E(mainTex6)
		E(mainTex7)
		E(matModelview)
		E(matMvp)
		E(matObjectToWorld)
		E(shininess)
		E(specular)
		E(invViewportSize)
	#undef E
};

