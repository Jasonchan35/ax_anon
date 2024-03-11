#pragma once

#if AX_USE_DX12

#if AX_OS_WINDOWS
	#include <d3d12.h>
	#include <dxgi1_6.h>

	#if _DEBUG
		#include <dxgidebug.h>
	#endif

	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")
#endif

#include "../../shader/axRenderState.h"
#include "../../axRenderCommonBase.h"

#if TRACY_ENABLE
#include <tracy/TracyD3D12.hpp>
#endif

class axDX12Material;
class axDX12Texture2D;
class axDX12Renderer;
class axDX12RenderContext;
class axDX12CommandDispatcher;

using axDX12_IDXGIFactory	= IDXGIFactory4;
using axDX12_ID3D12Device	= ID3D12Device;
using axDX12_IDXGIAdapter	= IDXGIAdapter3;

#if _DEBUG
	using axDX12_IDXGIDebug		= IDXGIDebug1;
	using axDX12_ID3D12Debug	= ID3D12Debug;
#endif

class axDX12Util : public axRenderCommonBase {
public:
	class DxResourceCreator;

	static bool isValid			(HRESULT hr) { if (!checkError(hr)) { reportError(hr); } }

	static void throwIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); throw axError_Undefined(AX_LOC); } }
	static void throwIfError	(HRESULT hr, ID3DBlob* error);

	static void assertIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); AX_ASSERT(false); } }
	static void reportError		(HRESULT hr);

	static bool checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}

	static axDX12Renderer* renderer();
	static axDX12_ID3D12Device*	d3dDevice();

	static D3D12_PRIMITIVE_TOPOLOGY			getDxPrimitiveTopology		(axRenderPrimitiveType t);
	static D3D12_PRIMITIVE_TOPOLOGY_TYPE	getDxPrimitiveTopologyType	(axRenderPrimitiveType t);
	static D3D12_FILTER						getDxTextureFilter			(axTextureFilter v);
	static D3D12_TEXTURE_ADDRESS_MODE		getDxTextureWrap			(axTextureWrap v);
	static DXGI_FORMAT						getDxIndexType				(axRenderIndexType t);
	static D3D12_COMPARISON_FUNC			getDxDepthTestOp			(axRenderState_DepthTestOp v);

	static DXGI_FORMAT		getDxColorType		(axColorType type);
	static DXGI_FORMAT		getDxDataType		(axRenderDataType v);
	static D3D12_BLEND_OP	getDxBlendOp		(axRenderState_BlendOp		v);
	static D3D12_BLEND		getDxBlendFactor	(axRenderState_BlendFactor	v);
	static D3D12_CULL_MODE	getDxCullMode		(axRenderState_Cull v);

	static axStrView	errorToStrView(ID3DBlob* blob);

	static UINT		castUINT	(axInt v) { return ax_safe_static_cast<UINT>(v); }
	static UINT16	castUINT16	(axInt v) { return ax_safe_static_cast<UINT16>(v); }
	static UINT64	castUINT64	(axInt v) { return ax_safe_static_cast<UINT64>(v); }
};

inline
void axDX12Util::throwIfError(HRESULT hr, ID3DBlob* error) {
	if (!checkError(hr)) {
		if (error) {
			AX_LOG("DX12 error: {?}", errorToStrView(error));
		}
		throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_PRIMITIVE_TOPOLOGY axDX12Util::getDxPrimitiveTopology(axRenderPrimitiveType t) {
	using SRC = axRenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case SRC::Lines:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case SRC::Triangles:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_PRIMITIVE_TOPOLOGY_TYPE axDX12Util::getDxPrimitiveTopologyType(axRenderPrimitiveType t) {
	using SRC = axRenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case SRC::Lines:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case SRC::Triangles:	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_FILTER axDX12Util::getDxTextureFilter(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case SRC::Linear:		return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case SRC::Bilinear:		return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case SRC::Trilinear:	return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case SRC::Anisotropic:	return D3D12_FILTER_ANISOTROPIC;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_TEXTURE_ADDRESS_MODE axDX12Util::getDxTextureWrap(axTextureWrap v) {
	using SRC = axTextureWrap;
	switch (v) {
		case SRC::Repeat:		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case SRC::Clamp:		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case SRC::Mirror:		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case SRC::MirrorOnce:	return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
DXGI_FORMAT axDX12Util::getDxIndexType(axRenderIndexType t) {
	using SRC = axRenderIndexType;
	switch (t) {
		case SRC::UInt16:	return DXGI_FORMAT_R16_UINT;
		case SRC::UInt32:	return DXGI_FORMAT_R32_UINT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline axStrView axDX12Util::errorToStrView(ID3DBlob* blob) {
	if (blob == nullptr)
		return axStrView();
	auto* s = static_cast<const char*>(blob->GetBufferPointer());
	auto  n = static_cast<axInt>(blob->GetBufferSize());
	return axStrView(s, n);
}

inline
DXGI_FORMAT axDX12Util::getDxColorType(axColorType type) {
	using SRC = axColorType;
	switch (type) {
		case SRC::HSBAf: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAf: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAh: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case SRC::RGBAb: return DXGI_FORMAT_R8G8B8A8_UNORM;
//		case SRC::RGBb:  
		case SRC::RGb:	 return DXGI_FORMAT_R8G8_UNORM;
		case SRC::Rb:	 return DXGI_FORMAT_R8_UNORM;
		case SRC::Ab:	 return DXGI_FORMAT_A8_UNORM;
		case SRC::Lb:	 return DXGI_FORMAT_R8_UNORM;
		case SRC::Af:	 return DXGI_FORMAT_R32_FLOAT;
		case SRC::Lf:	 return DXGI_FORMAT_R32_FLOAT;
		case SRC::LAf:	 return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::LAb:	 return DXGI_FORMAT_R8G8_UNORM;
//
		case SRC::BC1:		return DXGI_FORMAT_BC1_UNORM;
		case SRC::BC2:		return DXGI_FORMAT_BC2_UNORM;
		case SRC::BC3:		return DXGI_FORMAT_BC3_UNORM;
		case SRC::BC4:		return DXGI_FORMAT_BC4_UNORM;
		case SRC::BC5:		return DXGI_FORMAT_BC5_UNORM;
		case SRC::BC6h:		return DXGI_FORMAT_BC6H_UF16;
		case SRC::BC7:		return DXGI_FORMAT_BC7_UNORM;
//
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
DXGI_FORMAT axDX12Util::getDxDataType(axRenderDataType v) {
	using SRC = axRenderDataType;
	switch (v) {
		case SRC::UInt8:		return DXGI_FORMAT_R8_UNORM; break;
		case SRC::UInt8x2:		return DXGI_FORMAT_R8G8_UNORM; break;
//		case SRC::UInt8x3:		return DXGI_FORMAT_R8G8B8_UNORM; break; //does not support in DX11
		case SRC::UInt8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM; break;
	//--
		case SRC::Float16:		return DXGI_FORMAT_R16_FLOAT; break;
		case SRC::Float16x2:	return DXGI_FORMAT_R16G16_FLOAT; break;
//		case SRC::Float16x3:	return DXGI_FORMAT_R16G16B16_FLOAT; break; //does not support in DX11
		case SRC::Float16x4:	return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
	//---
		case SRC::Float32:		return DXGI_FORMAT_R32_FLOAT; break;
		case SRC::Float32x2:	return DXGI_FORMAT_R32G32_FLOAT; break;
		case SRC::Float32x3:	return DXGI_FORMAT_R32G32B32_FLOAT; break;
		case SRC::Float32x4:	return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_BLEND_OP axDX12Util::getDxBlendOp(axRenderState_BlendOp v) {
	using SRC = axRenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return D3D12_BLEND_OP_ADD;
		case SRC::Sub:		return D3D12_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return D3D12_BLEND_OP_REV_SUBTRACT;
		case SRC::Min:		return D3D12_BLEND_OP_MIN;
		case SRC::Max:		return D3D12_BLEND_OP_MAX;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_BLEND axDX12Util::getDxBlendFactor(axRenderState_BlendFactor v) {
	using SRC = axRenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:						return D3D12_BLEND_ZERO;
		case SRC::One:						return D3D12_BLEND_ONE;
		case SRC::SrcAlpha:					return D3D12_BLEND_SRC_ALPHA;
		case SRC::DstAlpha:					return D3D12_BLEND_DEST_ALPHA;
		case SRC::SrcColor:					return D3D12_BLEND_SRC_COLOR;
		case SRC::DstColor:					return D3D12_BLEND_DEST_COLOR;
		case SRC::ConstColor:				return D3D12_BLEND_BLEND_FACTOR;
//		case SRC::ConstAlpha:				return
		case SRC::OneMinusSrcAlpha:			return D3D12_BLEND_INV_SRC_ALPHA;
		case SRC::OneMinusSrcColor:			return D3D12_BLEND_INV_SRC_COLOR;
		case SRC::OneMinusDstAlpha:			return D3D12_BLEND_INV_DEST_ALPHA;
		case SRC::OneMinusDstColor:			return D3D12_BLEND_INV_DEST_COLOR;
		case SRC::OneMinusConstColor:		return D3D12_BLEND_INV_BLEND_FACTOR;
//		case SRC::OneMinusConstAlpha:		return
		case SRC::SrcAlphaSaturate:			return D3D12_BLEND_SRC_ALPHA_SAT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_CULL_MODE axDX12Util::getDxCullMode(axRenderState_Cull v) {
	using SRC = axRenderState_Cull;
	switch (v) {
		case SRC::None:	return D3D12_CULL_MODE_NONE;
		case SRC::Back:	return D3D12_CULL_MODE_BACK;
		case SRC::Front:	return D3D12_CULL_MODE_FRONT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D12_COMPARISON_FUNC axDX12Util::getDxDepthTestOp(axRenderState_DepthTestOp v) {
	using SRC = axRenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return  D3D12_COMPARISON_FUNC_ALWAYS;
		case SRC::Less:			return  D3D12_COMPARISON_FUNC_LESS;
		case SRC::Equal:		return  D3D12_COMPARISON_FUNC_EQUAL;
		case SRC::LessEqual:	return  D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case SRC::Greater:		return  D3D12_COMPARISON_FUNC_GREATER;
		case SRC::GreaterEqual:	return  D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case SRC::NotEqual:		return  D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case SRC::Never:		return  D3D12_COMPARISON_FUNC_NEVER;
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_DX12
