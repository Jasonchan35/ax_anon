#pragma once

#if AX_USE_VULKAN

#include <vulkan/vulkan.h>

#if AX_OS_WINDOWS
	#include <vulkan/vulkan_win32.h>

	#pragma comment(lib, "vulkan-1.lib")
//	#pragma comment(lib, "VkLayer_utils.lib")
#endif

#if AX_OS_OSX
	#import <MetalKit/MetalKit.h>
	#include <vulkan/vulkan_macos.h>
	#include <vulkan/vulkan_metal.h>
#endif

#if AX_USE_X11
	#include <vulkan/vulkan_xlib.h>
#endif

#define ax_Vulkan_TODO

#include "../../vertex/axRenderVertexDesc.h"
#include "../../shader/axRenderState.h"
#include "../../texture/axTexture.h"
#include "../../axRenderCommonBase.h"

#if TRACY_ENABLE
#include <tracy/TracyVulkan.hpp>
#endif

class axCppVkDevice;
class axCppVkSurfaceKHR;

class axVkCommandDispatcher;

class axVkRenderer;
class axVkRenderContext;
class axVkRenderContext_Base;
class axVkRenderCommandDispatcher;
class axVkShader;
class axVkShaderPass;
class axVkMaterial;
class axVkMaterialPass;

class axVkError : public axError {};
class axVkError_Undefined : public axVkError {};

#define axVkProcList_EnumList(E) \
	E(vkCreateDebugReportCallbackEXT)	\
//----------
/*
	E(vkDestroyDebugReportCallbackEXT)	\
	E(vkCmdSetCheckpointNV)	\
	E(vkGetQueueCheckpointDataNV) \
	E(vkCreateSwapchainKHR) \
*/

struct axVkProcList : public axNonCopyable {
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;

	#define AX_MACRO_OP(E) \
		PFN_##E E = nullptr; \
	//------
		axVkProcList_EnumList(AX_MACRO_OP);
	#undef AX_MACRO_OP

	void init(axVkRenderer* renderer);
};

class axVkAllocatorCallbacks : public VkAllocationCallbacks {
public:
	static axVkAllocatorCallbacks* s_instance() {
		static axVkAllocatorCallbacks s;
		return &s;
	}

	axVkAllocatorCallbacks();

private:
	static void* s_alloc			(void* pUserData,                  size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void* s_realloc			(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void  s_free				(void* pUserData, void* pMemory);
	static void  s_allocNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
	static void  s_freeNotification	(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
};

struct axVkQueueFamilyIndices {
	static const uint32_t kInvalidIndex = UINT32_MAX;

	bool presentQueueIsSeparated() const { return _graphIndex != _presentIndex; }
	void create(axCppVkSurfaceKHR& surface);

	uint32_t graphIndex() const		{ return _graphIndex; }
	uint32_t presentIndex() const	{ return _presentIndex; }

	bool operator==(const axVkQueueFamilyIndices& r) const {
		return _graphIndex	 == r._graphIndex
			&& _presentIndex == r._presentIndex;
	}

	bool operator!=(const axVkQueueFamilyIndices& r) const { return !operator==(r); }

private:
	uint32_t _graphIndex	= kInvalidIndex;
	uint32_t _presentIndex	= kInvalidIndex;
};

enum class axVkBufferIndexUsage : uint32_t {
	// UniformBuffer / StorageBuffer 0 ~ 15
	VertexAttr = 16,
};

class axVkUtil : public axRenderCommonBase {
public:
	static axCppVkDevice& rootDevice();

	static bool checkResult(VkResult res) {
		return res == VK_SUCCESS;
	}

	static void throwIfError(VkResult res) {
		if (!checkResult(res)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	static VkShaderStageFlags	getStageFlags(axShaderStageFlags f);

	static VkPrimitiveTopology	getVkPrimitiveTopology	(axRenderPrimitiveType t);

	static VkIndexType			getVkIndexType		(axRenderIndexType t);
	static VkCompareOp			getVkDepthTestOp	(axRenderState_DepthTestOp v);

	static VkFormat				getVkColorType		(axColorType type);
	static VkFormat				getVkDataType		(axRenderDataType v);

	static VkFilter				getVkTextureFilter		(axTextureFilter v);
	static VkSamplerMipmapMode	getVkSamplerMipmapMode	(axTextureFilter v);
	static VkSamplerAddressMode getVkTextureWrap		(axTextureWrap v);

	static VkBlendOp			getVkBlendOp		(axRenderState_BlendOp		v);
	static VkBlendFactor		getVkBlendFactor	(axRenderState_BlendFactor	v);
	static VkCullModeFlagBits	getVkCullMode		(axRenderState_Cull v);

	static axVkRenderer*		renderer();

	static bool formatHasDepth	(VkFormat f);
	static bool formatHasStencil(VkFormat f);


	static uint32_t		castUInt32(axInt v) 		{ return ax_safe_static_cast<uint32_t>(v); }
	static VkDeviceSize	castVkDeviceSize(axInt v) 	{ return ax_safe_static_cast<VkDeviceSize>(v); }

	static axVkAllocatorCallbacks* allocCallbacks() { 
		return nullptr;
	//	return axVkAllocatorCallbacks::s_instance();
	}
};

template<> inline
VkExtent2D ax_min<VkExtent2D>(const VkExtent2D& a, const VkExtent2D& b) {
	VkExtent2D o;
	o.width  = ax_min(a.width,  b.width );
	o.height = ax_min(a.height, b.height);
	return o;
}

template<> inline
VkExtent2D ax_max<VkExtent2D>(const VkExtent2D& a, const VkExtent2D& b) {
	VkExtent2D o;
	o.width  = ax_max(a.width,  b.width );
	o.height = ax_max(a.height, b.height);
	return o;
}

inline
VkShaderStageFlags axVkUtil::getStageFlags(axShaderStageFlags f) {
	using SRC = axShaderStageFlags;
	VkShaderStageFlags o = 0;
	if (ax_enum(f).hasFlags(SRC::Vertex	)) o |= VK_SHADER_STAGE_VERTEX_BIT;
	if (ax_enum(f).hasFlags(SRC::Pixel	)) o |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if (ax_enum(f).hasFlags(SRC::Compute)) o |= VK_SHADER_STAGE_COMPUTE_BIT;
	return o;
}

inline
VkPrimitiveTopology axVkUtil::getVkPrimitiveTopology(axRenderPrimitiveType t) {
	switch (t) {
		case axRenderPrimitiveType::Points:			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case axRenderPrimitiveType::Lines:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case axRenderPrimitiveType::Triangles:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
VkFormat axVkUtil::getVkColorType(axColorType type) {
	using SRC = axColorType;
	switch (type) {
		case SRC::HSBAf:	return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAf:	return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAh:	return VK_FORMAT_R16G16B16A16_SFLOAT;
		case SRC::RGBAb:	return VK_FORMAT_R8G8B8A8_UNORM;
//		case SRC::RGBb:
		case SRC::RGb:		return VK_FORMAT_R8G8_UNORM;
		case SRC::Rb:		return VK_FORMAT_R8_UNORM;
//		case SRC::Ab:		return VK_FORMAT_A8_UNORM;
		case SRC::Lb:		return VK_FORMAT_R8_UNORM;
		case SRC::Af:		return VK_FORMAT_R32_SFLOAT;
		case SRC::Lf:		return VK_FORMAT_R32_SFLOAT;
		case SRC::LAf:		return VK_FORMAT_R32G32_SFLOAT;
		case SRC::LAb:		return VK_FORMAT_R8G8_UNORM;
//
		case SRC::BC1:		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case SRC::BC2:		return VK_FORMAT_BC2_UNORM_BLOCK;
		case SRC::BC3:		return VK_FORMAT_BC3_UNORM_BLOCK;
		case SRC::BC4:		return VK_FORMAT_BC4_UNORM_BLOCK;
		case SRC::BC5:		return VK_FORMAT_BC5_UNORM_BLOCK;
		case SRC::BC6h:		return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case SRC::BC7:		return VK_FORMAT_BC7_UNORM_BLOCK;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
VkFormat axVkUtil::getVkDataType(axRenderDataType v) {
	using SRC = axRenderDataType;
	switch (v) {
		case SRC::UInt8:		return VK_FORMAT_R8_UNORM;				break;
		case SRC::UInt8x2:		return VK_FORMAT_R8G8_UNORM;			break;
	//	case SRC::UInt8x3:		return VK_FORMAT_R8G8B8_UNORM			break; //does not support in DX11
		case SRC::UInt8x4:		return VK_FORMAT_R8G8B8A8_UNORM;		break;
	//---
		case SRC::Float16:		return VK_FORMAT_R16_SFLOAT;			break;
		case SRC::Float16x2:	return VK_FORMAT_R16G16_SFLOAT;			break;
//		case SRC::Float16x3:	return VK_FORMAT_R16G16B16_SFLOAT;		break; //does not support in DX11
		case SRC::Float16x4:	return VK_FORMAT_R16G16B16A16_SFLOAT;	break;
	//---
		case SRC::Float32:		return VK_FORMAT_R32_SFLOAT;			break;
		case SRC::Float32x2:	return VK_FORMAT_R32G32_SFLOAT;			break;
		case SRC::Float32x3:	return VK_FORMAT_R32G32B32_SFLOAT;		break;
		case SRC::Float32x4:	return VK_FORMAT_R32G32B32A32_SFLOAT;	break;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkFilter axVkUtil::getVkTextureFilter(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return VK_FILTER_NEAREST;
		case SRC::Linear:		return VK_FILTER_NEAREST;
		case SRC::Bilinear:		return VK_FILTER_LINEAR;
		case SRC::Trilinear:	return VK_FILTER_LINEAR;
		case SRC::Anisotropic:	return VK_FILTER_LINEAR;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkSamplerMipmapMode axVkUtil::getVkSamplerMipmapMode(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SRC::Linear:		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case SRC::Bilinear:		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SRC::Trilinear:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case SRC::Anisotropic:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkSamplerAddressMode axVkUtil::getVkTextureWrap(axTextureWrap v) {
	using SRC = axTextureWrap;
	switch (v) {
		case SRC::Repeat:		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case SRC::Clamp:		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case SRC::Mirror:		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case SRC::MirrorOnce:	return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkBlendOp axVkUtil::getVkBlendOp(axRenderState_BlendOp v) {
	using SRC = axRenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return VK_BLEND_OP_ADD;
		case SRC::Sub:		return VK_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return VK_BLEND_OP_REVERSE_SUBTRACT;
		case SRC::Min:		return VK_BLEND_OP_MIN;
		case SRC::Max:		return VK_BLEND_OP_MAX;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkBlendFactor axVkUtil::getVkBlendFactor(axRenderState_BlendFactor v) {
	using SRC = axRenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:						return VK_BLEND_FACTOR_ZERO;
		case SRC::One:						return VK_BLEND_FACTOR_ONE;
		case SRC::SrcAlpha:					return VK_BLEND_FACTOR_SRC_ALPHA;
		case SRC::DstAlpha:					return VK_BLEND_FACTOR_DST_ALPHA;
		case SRC::SrcColor:					return VK_BLEND_FACTOR_SRC_COLOR;
		case SRC::DstColor:					return VK_BLEND_FACTOR_DST_COLOR;
		case SRC::ConstColor:				return VK_BLEND_FACTOR_CONSTANT_COLOR;
			//	case SRC::ConstAlpha:				return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case SRC::OneMinusSrcAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case SRC::OneMinusSrcColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case SRC::OneMinusDstAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case SRC::OneMinusDstColor:			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case SRC::OneMinusConstColor:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			//	case SRC::OneMinusConstAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case SRC::SrcAlphaSaturate:			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline VkCullModeFlagBits axVkUtil::getVkCullMode(axRenderState_Cull v) {
	using SRC = axRenderState_Cull;
	switch (v) {
		case SRC::None:		return VK_CULL_MODE_NONE;
		case SRC::Back:		return VK_CULL_MODE_BACK_BIT;
		case SRC::Front:	return VK_CULL_MODE_FRONT_BIT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
VkIndexType axVkUtil::getVkIndexType(axRenderIndexType t) {
	using SRC = axRenderIndexType;
	switch (t) {
		case SRC::UInt16:	return VK_INDEX_TYPE_UINT16;
		case SRC::UInt32:	return VK_INDEX_TYPE_UINT32;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
VkCompareOp axVkUtil::getVkDepthTestOp(axRenderState_DepthTestOp v) {
	using SRC = axRenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return  VK_COMPARE_OP_ALWAYS;
		case SRC::Less:			return  VK_COMPARE_OP_LESS;
		case SRC::Equal:		return  VK_COMPARE_OP_EQUAL;
		case SRC::LessEqual:	return  VK_COMPARE_OP_LESS_OR_EQUAL;
		case SRC::Greater:		return  VK_COMPARE_OP_GREATER;
		case SRC::GreaterEqual:	return  VK_COMPARE_OP_GREATER_OR_EQUAL;
		case SRC::NotEqual:		return  VK_COMPARE_OP_NOT_EQUAL;
		case SRC::Never:		return  VK_COMPARE_OP_NEVER;
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_VULKAN
