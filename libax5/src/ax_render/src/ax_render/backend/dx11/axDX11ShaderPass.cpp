#if AX_USE_DX11

#include "axDX11Shader.h"
#include "axDX11ShaderPass.h"
#include "axDX11Renderer.h"
#include "axDX11CommandDispatcher.h"
#include "../../axRenderDrawCall.h"

void axDX11ShaderPass::onInit(axInt passIndex, const Info& info) {
	_vertexShader.unref();
	_pixelShader.unref();
	_computeShader.unref();

	auto shaderFilename = shader()->filename();

	HRESULT hr = 0;

	auto* d3dDevice = Util::d3dDevice();

	axTempString vsBytecodeFilename;
	axTempString psBytecodeFilename;
	axTempString csBytecodeFilename;

	vsBytecodeFilename.format("{?}/DX11_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX11_VS);
	psBytecodeFilename.format("{?}/DX11_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX11_PS);
	csBytecodeFilename.format("{?}/DX11_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX11_CS);

	axTempString vsInfoFilename(vsBytecodeFilename, ".json");
	axTempString psInfoFilename(psBytecodeFilename, ".json");
	axTempString csInfoFilename(csBytecodeFilename, ".json");

	if (info.vsFunc) {
		axFile::readBytes(vsBytecodeFilename, _vsBytecode);
		hr = d3dDevice->CreateVertexShader(_vsBytecode.data(), _vsBytecode.size(), nullptr, _vertexShader.ptrForInit());
		Util::throwIfError(hr);
	}

	if (info.psFunc) {
		axFile::readBytes(psBytecodeFilename, _psBytecode);
		hr = d3dDevice->CreatePixelShader(_psBytecode.data(), _psBytecode.size(), nullptr, _pixelShader.ptrForInit());
		Util::throwIfError(hr);
	}

	if (info.csFunc) {
		axFile::readBytes(csBytecodeFilename, _csBytecode);
		hr = d3dDevice->CreateComputeShader(_csBytecode.data(), _csBytecode.size(), nullptr, _computeShader.ptrForInit());
		Util::throwIfError(hr);
	}

	initStageInfos(info, vsInfoFilename, psInfoFilename, csInfoFilename);
}

bool axDX11ShaderPass::onValidate() {
	return true;
}

void axDX11ShaderPass::_bindVertexInputLayout(axDX11CommandDispatcher& dispatcher, VertexDesc vertexDesc) {
	checkAcceptVertexDesc(vertexDesc);

	auto* p = _vertexInputLayoutMap.find(vertexDesc->vertexType);
	if (!p) {
		p = &_vertexInputLayoutMap.add(vertexDesc->vertexType);
		VertexInputLayoutDesc	layoutDesc(_vertexAttrs, vertexDesc);

		auto* d3dDevice = Util::d3dDevice();
		auto hr = d3dDevice->CreateInputLayout(	layoutDesc.desc.data(),
												Util::castUINT(layoutDesc.desc.size()),
												_vsBytecode.data(), _vsBytecode.size(), 
												p->ptrForInit());
		Util::throwIfError(hr);
	}

	auto* layout = p->ptr();
	if (!layout) throw axError_Undefined(AX_LOC);

	dispatcher.d3dContext->IASetInputLayout(layout);
}

axDX11ShaderPass::VertexInputLayoutDesc::VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc) {
	for (auto& reqAttr : requiredVertexAttrs) {
		auto attrId = reqAttr.attrId;
		auto& attr = inputVertexDesc->attr(attrId);

		axShaderPass::validateVertexAttr(reqAttr, attr);

		auto& dst = desc.emplaceBack();

		if (attrId >= VertexAttrId::TEXCOORD0 && attrId < VertexAttrId::TEXCOORD_END) {
			dst.SemanticName = "TEXCOORD";
			dst.SemanticIndex = ax_enum_int(attrId - VertexAttrId::TEXCOORD0);

		}else if (attrId >= VertexAttrId::CUSTOM0 && attrId < VertexAttrId::CUSTOM_END) {
			dst.SemanticName = "TEXCOORD"; // CUSTOM also use TEXCOORD
			dst.SemanticIndex = Util::castUINT(ax_enum_int(attrId - VertexAttrId::CUSTOM0) + axRenderVertexAttrId_kUvCount);

		}else if (attrId >= VertexAttrId::COLOR0 && attrId < VertexAttrId::COLOR_END) {
			dst.SemanticName = "COLOR";
			dst.SemanticIndex = ax_enum_int(attrId - VertexAttrId::COLOR0);

		}else{
			switch (attrId) {
				case VertexAttrId::POSITION0: dst.SemanticName = "POSITION"; break;
				case VertexAttrId::NORMAL0:   dst.SemanticName = "NORMAL";   break;
				case VertexAttrId::TANGENT0:  dst.SemanticName = "TANGENT";  break;
				default: throw axError_Undefined(AX_LOC);
			}
			dst.SemanticIndex = 0;
		}

		dst.InputSlot = 0;
		dst.AlignedByteOffset = attr.offset;
		
		dst.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
		dst.Format = Util::getDxDataType(attr.dataType);
	}
}

void axDX11ShaderPass::bind(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {
	_bindVertexInputLayout(dispatcher, drawCall.vertexDesc);
	_bindRenderState(dispatcher, drawCall.debugWireframe);

	dispatcher.d3dContext->VSSetShader(_vertexShader, nullptr, 0);
	dispatcher.d3dContext->PSSetShader(_pixelShader,  nullptr, 0);
}

void axDX11ShaderPass::bind(axDX11CommandDispatcher& dispatcher, ComputeCall& computeCall) {
	dispatcher.d3dContext->CSSetShader(_computeShader, nullptr, 0);
}

void axDX11ShaderPass::_createRasterState(Rso& rs, const RsoKey& key) {
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.AntialiasedLineEnable = _staticRenderState.antialiasedLine;
	rasterDesc.CullMode = Util::getDxCullMode(_staticRenderState.cull);
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = key.debugWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	auto* dev = Util::d3dDevice();
	auto hr = dev->CreateRasterizerState(&rasterDesc, rs.ptrForInit());
	Util::throwIfError(hr);
}

void axDX11ShaderPass::_bindRenderState(axDX11CommandDispatcher& dispatcher, bool debugWireframe) {
	auto* dev = Util::d3dDevice();

	RsoKey key;
	key.debugWireframe = debugWireframe || _staticRenderState.wireframe;

	auto* rs = _rasterStateMap.find(key);
	if (!rs) {
		rs = &_rasterStateMap.add(key);
		_createRasterState(*rs, key);
	}
	dispatcher.d3dContext->RSSetState(*rs);

	if (!_depthStencilState) {
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		if (_staticRenderState.depthTest.isEnable()) {
			depthStencilDesc.DepthEnable	= true;
			depthStencilDesc.DepthFunc		= Util::getDxDepthTestOp(_staticRenderState.depthTest.op);
		} else {
			depthStencilDesc.DepthEnable	= false;
		}

		depthStencilDesc.DepthWriteMask		= _staticRenderState.depthTest.writeMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.StencilEnable		= false;
		depthStencilDesc.StencilReadMask	= 0xFF;
		depthStencilDesc.StencilWriteMask	= 0xFF;

		auto hr = dev->CreateDepthStencilState(&depthStencilDesc, _depthStencilState.ptrForInit());
		Util::throwIfError(hr);
	}
	dispatcher.d3dContext->OMSetDepthStencilState(_depthStencilState, 1);

	if (!_blendState) {
		auto& blend = _staticRenderState.blend;

		D3D11_BLEND_DESC blendStateDesc = {};
		blendStateDesc.AlphaToCoverageEnable  = false;
		blendStateDesc.IndependentBlendEnable = false;
		auto& rtDesc = blendStateDesc.RenderTarget[0];

		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (blend.isEnable()) {
			rtDesc.BlendEnable	  = true;
			rtDesc.BlendOp        = Util::getDxBlendOp(blend.rgb.op);
			rtDesc.BlendOpAlpha   = Util::getDxBlendOp(blend.alpha.op);
			rtDesc.SrcBlend       = Util::getDxBlendFactor(blend.rgb.srcFactor);
			rtDesc.DestBlend      = Util::getDxBlendFactor(blend.rgb.dstFactor);
			rtDesc.SrcBlendAlpha  = Util::getDxBlendFactor(blend.alpha.srcFactor);
			rtDesc.DestBlendAlpha = Util::getDxBlendFactor(blend.alpha.dstFactor);
		}else{
			rtDesc.BlendEnable	  = false;
		}

		auto hr = dev->CreateBlendState(&blendStateDesc, _blendState.ptrForInit());
		Util::throwIfError(hr);
	}

	auto color = axColorRGBAf(_staticRenderState.blend.constColor);
	dispatcher.d3dContext->OMSetBlendState(_blendState, color.data(), 0xffffffff);
}

#endif //#if AX_USE_DX11
