#pragma once

#include "axDX11CommonBase.h"

#if AX_USE_DX11

#include "../../shader/axShader.h"

class axDX11CommandDispatcher;

class axDX11ShaderPass : public axShaderPass {
	AX_RTTI_CLASS(axDX11ShaderPass, axShaderPass);
public:
	using Util = axDX11Util;

	virtual	void onInit(axInt passIndex, const Info& info) override;
	virtual bool onValidate	() override;

	axByteArray			_vsBytecode;
	axByteArray			_psBytecode;
	axByteArray			_csBytecode;

	axComPtr<ID3D11VertexShader>	_vertexShader;
	axComPtr<ID3D11PixelShader>		_pixelShader;
	axComPtr<ID3D11ComputeShader>	_computeShader;

	void bind	(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {}

	void bind	(axDX11CommandDispatcher& dispatcher, ComputeCall& computeCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, ComputeCall& computeCall) {}

	axArrayMap<axRenderVertexType, axComPtr<ID3D11InputLayout>, 8> _vertexInputLayoutMap;

	class VertexInputLayoutDesc {
	public:
		VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc);
		axArray<D3D11_INPUT_ELEMENT_DESC, 64>	desc;
	};

	class RsoKey {
	public:
		bool debugWireframe = false;

		bool operator==(const RsoKey& r) const {
			return debugWireframe	== r.debugWireframe;
		}
	};

private:
	using Rso = axComPtr<ID3D11RasterizerState>;

	void _createRasterState		(Rso& outState, const RsoKey& key);
	void _bindVertexInputLayout	(axDX11CommandDispatcher& dispatcher, VertexDesc meshDesc);
	void _bindRenderState		(axDX11CommandDispatcher& dispatcher, bool debugWireframe);

	axArrayMap<RsoKey, Rso, 4> _rasterStateMap;

	axComPtr<ID3D11DepthStencilState>	_depthStencilState;
	axComPtr<ID3D11BlendState>			_blendState;

};

#endif //AX_USE_DX11
