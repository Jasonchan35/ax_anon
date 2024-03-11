#pragma once

#if AX_USE_DX11

#if AX_OS_WINDOWS
	#include <d3d11.h>
	#include <d3d11_4.h>
	#include <dxgi1_4.h>
	#pragma comment(lib, "d3d11.lib")
#endif

#include "../../axRenderCommonBase.h"
#include "../../texture/axSamplerState.h"

class axDX11Renderer;
class axDX11Texture2D;
class axDX11CommandDispatcher;
class axDX11RenderContext;

using axDX11_ID3DDevice			= ID3D11Device1;
using axDX11_ID3DDeviceContext	= ID3D11DeviceContext4;
using axDX11_IDXGIFactory		= IDXGIFactory1;
using axDX11_ID3D11Debug		= ID3D11Debug;

using axDX11_IDXGIDevice		= IDXGIDevice;
using axDX11_IDXGIAdapter		= IDXGIAdapter3;

class axDX11Util : public axRenderCommonBase {
public:

	static void throwIfError	(HRESULT hr) { 
		if (!_checkError(hr)) { reportError(); throw axError_Undefined(AX_LOC); }
	}
	static bool assertIfError	(HRESULT hr) {
		if (!_checkError(hr)) { reportError(); AX_ASSERT(false); return false; }
		return true;
	}
	static void reportError();

	static D3D11_PRIMITIVE_TOPOLOGY		getDxPrimitiveTopology	(axRenderPrimitiveType t);
	static DXGI_FORMAT					getDxIndexType			(axRenderIndexType t);
	static D3D11_BLEND_OP				getDxBlendOp			(axRenderState_BlendOp v);
	static D3D11_BLEND					getDxBlendFactor		(axRenderState_BlendFactor v);
	static D3D11_CULL_MODE				getDxCullMode			(axRenderState_Cull v);
	static D3D11_COMPARISON_FUNC		getDxDepthTestOp		(axRenderState_DepthTestOp v);
	static DXGI_FORMAT					getDxColorType			(axColorType type);
	static DXGI_FORMAT					getDxDataType			(axRenderDataType v);

	static D3D11_FILTER					getDxTextureFilter		(axTextureFilter v);
	static D3D11_TEXTURE_ADDRESS_MODE	getDxTextureWrap		(axTextureWrap   v);


	static UINT castUINT(axInt v) { return ax_safe_static_cast<UINT>(v); }

	static axDX11Renderer*				renderer();
	static axDX11_ID3DDevice*			d3dDevice();
	static axDX11_ID3DDeviceContext*	d3dContext();

private:
	static bool _checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}
};

inline
void axDX11Util::reportError() {
#if 0 && _DEBUG
	auto* d = renderer()->d3dDebug();
	if (d) {
		d->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
}

inline
DXGI_FORMAT axDX11Util::getDxColorType(axColorType type) {
	using SRC = axColorType;
	switch (type) {
		case SRC::HSBAf:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAf:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAh:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case SRC::RGBAb:	return DXGI_FORMAT_R8G8B8A8_UNORM;
//		case SRC::RGBb:		
		case SRC::RGb:		return DXGI_FORMAT_R8G8_UNORM;
		case SRC::Rb:		return DXGI_FORMAT_R8_UNORM;
		case SRC::Ab:		return DXGI_FORMAT_A8_UNORM;
		case SRC::Lb:		return DXGI_FORMAT_R8_UNORM;
		case SRC::Af:		return DXGI_FORMAT_R32_FLOAT;
		case SRC::Lf:		return DXGI_FORMAT_R32_FLOAT;
		case SRC::LAf:		return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::LAb:		return DXGI_FORMAT_R8G8_UNORM;
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
DXGI_FORMAT axDX11Util::getDxDataType(axRenderDataType v) {
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

inline D3D11_FILTER axDX11Util::getDxTextureFilter(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case SRC::Linear:		return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case SRC::Bilinear:		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case SRC::Trilinear:	return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case SRC::Anisotropic:	return D3D11_FILTER_ANISOTROPIC;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline D3D11_TEXTURE_ADDRESS_MODE axDX11Util::getDxTextureWrap(axTextureWrap v) {
	using SRC = axTextureWrap;
	switch (v) {
		case SRC::Repeat:		return D3D11_TEXTURE_ADDRESS_WRAP;
		case SRC::Clamp:		return D3D11_TEXTURE_ADDRESS_CLAMP;
		case SRC::Mirror:		return D3D11_TEXTURE_ADDRESS_MIRROR;
		case SRC::MirrorOnce:	return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D11_PRIMITIVE_TOPOLOGY axDX11Util::getDxPrimitiveTopology(axRenderPrimitiveType t) {
	switch (t) {
		case axRenderPrimitiveType::Points:				return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case axRenderPrimitiveType::Lines:				return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case axRenderPrimitiveType::Triangles:			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
DXGI_FORMAT axDX11Util::getDxIndexType(axRenderIndexType t) {
	switch (t) {
		case axRenderIndexType::UInt16:	return DXGI_FORMAT_R16_UINT;
		case axRenderIndexType::UInt32:	return DXGI_FORMAT_R32_UINT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D11_BLEND_OP axDX11Util::getDxBlendOp(axRenderState_BlendOp v) {
	using SRC = axRenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return D3D11_BLEND_OP_ADD;
		case SRC::Min:		return D3D11_BLEND_OP_MIN;
		case SRC::Max:		return D3D11_BLEND_OP_MAX;
		case SRC::Sub:		return D3D11_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return D3D11_BLEND_OP_REV_SUBTRACT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D11_BLEND axDX11Util::getDxBlendFactor(axRenderState_BlendFactor v) {
	using SRC = axRenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:					return D3D11_BLEND_ZERO;
		case SRC::One:					return D3D11_BLEND_ONE;
		case SRC::SrcAlpha:				return D3D11_BLEND_SRC_ALPHA;
		case SRC::DstAlpha:				return D3D11_BLEND_DEST_ALPHA;
		case SRC::SrcColor:				return D3D11_BLEND_SRC_COLOR;
		case SRC::DstColor:				return D3D11_BLEND_DEST_COLOR;
		case SRC::ConstColor:			return D3D11_BLEND_BLEND_FACTOR;
//		case SRC::ConstAlpha:			return 
		case SRC::OneMinusSrcAlpha:		return D3D11_BLEND_INV_SRC_ALPHA;
		case SRC::OneMinusSrcColor:		return D3D11_BLEND_INV_SRC_COLOR;
		case SRC::OneMinusDstAlpha:		return D3D11_BLEND_INV_DEST_ALPHA;
		case SRC::OneMinusDstColor:		return D3D11_BLEND_INV_DEST_COLOR;
		case SRC::OneMinusConstColor:	return D3D11_BLEND_INV_BLEND_FACTOR;
//		case SRC::OneMinusConstAlpha:	return 
		case SRC::SrcAlphaSaturate:		return D3D11_BLEND_SRC_ALPHA_SAT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D11_CULL_MODE axDX11Util::getDxCullMode(axRenderState_Cull v) {
	using SRC = axRenderState_Cull;
	switch (v) {
		case SRC::Back:		return D3D11_CULL_BACK;
		case SRC::Front:	return D3D11_CULL_FRONT;
		case SRC::None:		return D3D11_CULL_NONE;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
D3D11_COMPARISON_FUNC axDX11Util::getDxDepthTestOp(axRenderState_DepthTestOp v) {
	using SRC = axRenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return  D3D11_COMPARISON_ALWAYS;
		case SRC::Less:			return  D3D11_COMPARISON_LESS;
		case SRC::Equal:		return  D3D11_COMPARISON_EQUAL;
		case SRC::LessEqual:	return  D3D11_COMPARISON_LESS_EQUAL;
		case SRC::Greater:		return  D3D11_COMPARISON_GREATER;
		case SRC::GreaterEqual:	return  D3D11_COMPARISON_GREATER_EQUAL;
		case SRC::NotEqual:		return  D3D11_COMPARISON_NOT_EQUAL;
		case SRC::Never:		return  D3D11_COMPARISON_NEVER;
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_DX11