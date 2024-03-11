#include "../../ax_render_config.h"

#if AX_USE_DX12

#include "axDX12ShaderPass.h"

void axDX12ShaderPass::onInit(axInt passIndex, const Info& info) {
	auto shaderFilename = shader()->filename();

	HRESULT hr = 0;

	axTempString vsBytecodeFilename;
	axTempString psBytecodeFilename;
	axTempString csBytecodeFilename;

	vsBytecodeFilename.format("{?}/DX12_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX12_VS);
	psBytecodeFilename.format("{?}/DX12_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX12_PS);
	csBytecodeFilename.format("{?}/DX12_pass{?}.{?}.bin", shaderFilename, passIndex, StageProfile::DX12_CS);

	axTempString vsInfoFilename(vsBytecodeFilename, ".json");
	axTempString psInfoFilename(psBytecodeFilename, ".json");
	axTempString csInfoFilename(csBytecodeFilename, ".json");

	if (info.vsFunc) axFile::readBytes(vsBytecodeFilename, _vsBytecode);
	if (info.psFunc) axFile::readBytes(psBytecodeFilename, _psBytecode);
	if (info.csFunc) axFile::readBytes(csBytecodeFilename, _csBytecode);

	initStageInfos(info, vsInfoFilename, psInfoFilename, csInfoFilename);

	onInit_DescriptorTable(_psoTexDescTable);
	onInit_DescriptorTable(_psoSamplerDescTable);
	onInit_DescriptorTable(_psoStorageBufDescTable);

	{	// Create an root signature.
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedDesc;
		versionedDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;

		auto& desc = versionedDesc.Desc_1_0;
		desc = {};

		if (!isCompute()) {
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		}
		desc.NumParameters		= Util::castUINT(_psoRootParams.size());
		desc.pParameters		= _psoRootParams.data();
//		desc.NumStaticSamplers	= Util::castUINT(_psoRootStaticSamplers.size());
//		desc.pStaticSamplers	= _psoRootStaticSamplers.data();

		axComPtr<ID3DBlob> signature;
		axComPtr<ID3DBlob> error;

		hr = D3D12SerializeVersionedRootSignature(&versionedDesc, signature.ptrForInit(), error.ptrForInit());
		Util::throwIfError(hr, error);

		auto* d3dDevice = Util::d3dDevice();
		hr = d3dDevice->CreateRootSignature(0, 
											signature->GetBufferPointer(), 
											signature->GetBufferSize(), 
											IID_PPV_ARGS(_rootSignature.ptrForInit()));
		Util::throwIfError(hr);
	}

	if (isCompute()) {
		_createComputePipelineState(_computePso);
	}
}

void axDX12ShaderPass::onInit_DescriptorTable(DescriptorTable& descTable) {
	if (descTable._ranges.size() <= 0) return;

	descTable._rootParamIndex = Util::castUINT(_psoRootParams.size());

	auto& dst = _psoRootParams.emplaceBack();
	dst.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	dst.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	dst.DescriptorTable.NumDescriptorRanges = Util::castUINT(descTable._ranges.size());
	dst.DescriptorTable.pDescriptorRanges = descTable._ranges.data();
}

void axDX12ShaderPass::onInitStageInfos(StageInfoSet& infoSet) {
	Base::onInitStageInfos(infoSet);
	D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	auto rootParamReserveCount	= _uniformBufferMap.size() 
								+ _texParams.size() ? 1 : 0
								+ _samplerStateMap.size() ? 1 : 0
								+ _storageBufParams.size() ? 1 : 0;

	_psoRootParams.reserve(rootParamReserveCount);
	
	for (auto& cb : _uniformBufferMap) {
		auto& dst = _psoRootParams.emplaceBack();
		dst.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		dst.ShaderVisibility = shaderVisibility;
		dst.Descriptor.ShaderRegister = Util::castUINT(cb.bindPoint);
		dst.Descriptor.RegisterSpace = 0;
	}

	_psoTexDescTable._ranges.reserve(_texParams.size());
	for (auto& tp : _texParams) {
		auto& p = _paramMap[tp.paramIndex];

		auto& dst = _psoTexDescTable._ranges.emplaceBack();
		dst.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		dst.NumDescriptors		= 1;
		dst.BaseShaderRegister	= Util::castUINT(p.bindPoint);
		dst.RegisterSpace		= 0;
		dst.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

#if 1
	_psoSamplerDescTable._ranges.reserve(_samplerStateMap.size());
	for (auto& s : _samplerStateMap) {

		auto& dst = _psoSamplerDescTable._ranges.emplaceBack();
		dst.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		dst.NumDescriptors		= 1;
		dst.BaseShaderRegister	= Util::castUINT(s.bindPoint);
		dst.RegisterSpace		= 0;
		dst.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

#else
	_psoRootStaticSamplers.reserve(_samplerStateMap.size());
	for (auto& s : _samplerStateMap) {
		auto& dst = _psoRootStaticSamplers.emplaceBack();
		dst.ShaderVisibility = shaderVisibility;
		dst.Filter   = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		dst.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		dst.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		dst.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		dst.MipLODBias = 0;
		dst.MaxAnisotropy = 0;
		dst.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		dst.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		dst.MinLOD = 0.0f;
		dst.MaxLOD = D3D12_FLOAT32_MAX;
		dst.ShaderRegister = Util::castUINT(s.bindPoint);
		dst.RegisterSpace = 0;
	}
#endif

	for (auto& sb : _storageBufParams) {
		auto& p = _paramMap[sb.paramIndex];

		auto& dst = _psoStorageBufDescTable._ranges.emplaceBack();
		dst.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		dst.NumDescriptors		= 1;
		dst.BaseShaderRegister	= Util::castUINT(p.bindPoint);
		dst.RegisterSpace		= 0;
		dst.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

}

void axDX12ShaderPass::bind(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall) {
	PsoKey key;
	key.vertexDesc = drawCall.vertexDesc;
	key.primitiveType = drawCall.primitiveType;
	key.debugWireframe = drawCall.debugWireframe || _staticRenderState.wireframe;

	auto* p = _pipelineStateMap.find(key);
	if (!p) {
		p = &_pipelineStateMap.add(key);
		_createPipelineState(*p, key);
	}

	auto* pso = p->ptr();
	if (!pso) throw axError_Undefined(AX_LOC);

	cmdList->SetGraphicsRootSignature(_rootSignature);
	cmdList->SetPipelineState(pso);
}

void axDX12ShaderPass::bind(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall) {
	cmdList->SetComputeRootSignature(_rootSignature);
	cmdList->SetPipelineState(_computePso);
}

void axDX12ShaderPass::_createComputePipelineState(Pso& outPso) {
	auto* d3dDevice = Util::d3dDevice();

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature			= _rootSignature;
	psoDesc.CS.pShaderBytecode		= _csBytecode.data();
	psoDesc.CS.BytecodeLength		= _csBytecode.sizeInBytes();
//	psoDesc.CachedPSO.pCachedBlob	= nullptr;
//	psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
//	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

	auto hr = d3dDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(outPso.ptrForInit()));
	Util::throwIfError(hr);
}

void axDX12ShaderPass::_createPipelineState(Pso& outPso, const PsoKey& key) {
	auto* d3dDevice = Util::d3dDevice();

	checkAcceptVertexDesc(key.vertexDesc);
	VertexInputLayoutDesc	vertexInputLayoutDesc(_vertexAttrs, key.vertexDesc);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout.NumElements = Util::castUINT(vertexInputLayoutDesc.desc.size());
	psoDesc.InputLayout.pInputElementDescs = vertexInputLayoutDesc.desc.data();
//-----
	psoDesc.pRootSignature     = _rootSignature;
	psoDesc.VS.pShaderBytecode = _vsBytecode.data();
	psoDesc.VS.BytecodeLength  = _vsBytecode.sizeInBytes();
	psoDesc.PS.pShaderBytecode = _psBytecode.data();
	psoDesc.PS.BytecodeLength  = _psBytecode.sizeInBytes();
//-----
	psoDesc.RasterizerState.FillMode = key.debugWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;

	psoDesc.RasterizerState.FrontCounterClockwise = true;
	psoDesc.RasterizerState.CullMode = Util::getDxCullMode(_staticRenderState.cull);

	psoDesc.DepthStencilState.DepthEnable		= _staticRenderState.depthTest.isEnable();
	psoDesc.DepthStencilState.DepthWriteMask	= _staticRenderState.depthTest.writeMask ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc			= Util::getDxDepthTestOp(_staticRenderState.depthTest.op);
	psoDesc.DepthStencilState.StencilEnable	= false;

	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
//-----
	{	// blend
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rt;
		rt.LogicOpEnable = false;

		auto& blend = _staticRenderState.blend;
		rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		if (blend.isEnable()) {
			rt.BlendEnable	= true;
			rt.BlendOp			= Util::getDxBlendOp(blend.rgb.op);
			rt.BlendOpAlpha		= Util::getDxBlendOp(blend.alpha.op);
			rt.SrcBlend			= Util::getDxBlendFactor(blend.rgb.srcFactor);
			rt.DestBlend		= Util::getDxBlendFactor(blend.rgb.dstFactor);
			rt.SrcBlendAlpha	= Util::getDxBlendFactor(blend.alpha.srcFactor);
			rt.DestBlendAlpha	= Util::getDxBlendFactor(blend.alpha.dstFactor);
		}else{
			rt.BlendEnable	= false;
		}

		psoDesc.BlendState.RenderTarget[0] = rt;

		rt.BlendEnable = false;
		for (UINT i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			psoDesc.BlendState.RenderTarget[ i ] = rt;
	}

//-----
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = Util::getDxPrimitiveTopologyType(key.primitiveType);
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	auto hr = d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(outPso.ptrForInit()));
	Util::throwIfError(hr);
}

axDX12ShaderPass::VertexInputLayoutDesc::VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, const VertexDesc& vertexDesc) {
	for (auto& reqAttr : requiredVertexAttrs) {
		auto attrId = reqAttr.attrId;
		auto& attr = vertexDesc->attr(attrId);

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
				case VertexAttrId::BINORMAL0: dst.SemanticName = "BINORMAL"; break;
				default: throw axError_Undefined(AX_LOC);
			}
			dst.SemanticIndex = 0;
		}

		dst.InputSlot = 0;
		dst.AlignedByteOffset = attr.offset;
		
		dst.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
		dst.Format = Util::getDxDataType(attr.dataType);
	}
}

#endif //AX_USE_DX12

