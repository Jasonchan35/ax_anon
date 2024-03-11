#pragma once

#include "axDX12CommandDispatcher.h"
#include "../../shader/axShader.h"

#if AX_USE_DX12

class axDX12RenderContext;
class axDX12ShaderPass : public axShaderPass {
	AX_RTTI_CLASS(axDX12ShaderPass, axShaderPass);
public:
	using Util = axDX12Util;

	virtual	void onInit(axInt passIndex, const Info& info) override;
	virtual bool onValidate	() override { return true; }

	axByteArray _vsBytecode;
	axByteArray _psBytecode;
	axByteArray _csBytecode;

	void bind	(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall);
	void bind	(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall);

	class VertexInputLayoutDesc {
	public:
		VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, const VertexDesc& vertexDesc);
		axArray<D3D12_INPUT_ELEMENT_DESC, 64>	desc;
	};

//---- Pipeline State Object ----
	using Pso = axComPtr<ID3D12PipelineState>;
	class PsoKey {
	public:
		VertexDesc vertexDesc = nullptr;
		PrimitiveType primitiveType = PrimitiveType::Unknown;
		bool debugWireframe = false;


		bool operator==(const PsoKey& r) const {
			return vertexDesc		== r.vertexDesc
				&& primitiveType	== r.primitiveType
				&& debugWireframe	== r.debugWireframe;
		}
	};

	class DescriptorTable {
	public:
		axArray<D3D12_DESCRIPTOR_RANGE, 8>		_ranges;
		UINT									_rootParamIndex = 0;
	};

	axArray<D3D12_ROOT_PARAMETER, 8>		_psoRootParams;
//	axArray<D3D12_STATIC_SAMPLER_DESC, 8>	_psoRootStaticSamplers;
	DescriptorTable							_psoTexDescTable;
	DescriptorTable							_psoSamplerDescTable;
	DescriptorTable							_psoStorageBufDescTable;

	void _createPipelineState(Pso& outPso, const PsoKey& key);
	axArrayMap<PsoKey, Pso, 4> _pipelineStateMap;

	void _createComputePipelineState(Pso& outPso);
	Pso _computePso;

//-------
	virtual void onInitStageInfos(StageInfoSet& infoSet);
			void onInit_DescriptorTable(DescriptorTable& descTable);

	axComPtr<ID3D12RootSignature>		_rootSignature;
};

#endif //AX_USE_DX12
