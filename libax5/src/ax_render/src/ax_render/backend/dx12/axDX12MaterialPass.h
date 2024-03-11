#pragma once

#if AX_USE_DX12

#include "../../shader/axMaterialPass.h"
#include "axDX12ShaderPass.h"
#include "axDX12DescripterHeap.h"

class axDX12MaterialPass : public axMaterialPass {
	AX_RTTI_CLASS(axDX12MaterialPass, axMaterialPass);
public:
	using Util = axDX12Util;

	void bind	(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall);
	void bind	(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall);

	void onSetShaderPass(axShaderPass* shaderPass);

	axDX12ShaderPass*	shaderPass() { return ax_type_cast_debug<axDX12ShaderPass>(Base::shaderPass()); }

private:
	void _bindParams(ID3D12GraphicsCommandList* cmdList);

	axDX12DescripterHeap_CBV_SRV_UAV	_texDescHeap;
	axDX12DescripterHeap_Sampler		_samplerDescHeap;
	axDX12DescripterHeap_CBV_SRV_UAV	_storageBufDescHeap;

};

#endif