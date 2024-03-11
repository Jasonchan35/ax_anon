#if AX_USE_DX12_DXR

#include "axDX12RayTracing.h"

bool axDX12RayTracing::create() {
	_renderer = renderer();
	auto* d3dDevice = _renderer->d3dDevice();
	HRESULT hr;

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};
	hr = d3dDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData));
	if (checkError(hr)) {
		if (featureSupportData.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
			return false;
	}

	hr = d3dDevice->QueryInterface(IID_PPV_ARGS(_dxrDevice.ptrForInit()));
	throwIfError(hr);

	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask	= 0;

		hr = _dxrDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_cmdQueue.ptrForInit()));
		throwIfError(hr);
	}

	hr = _dxrDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ptrForInit()));
	throwIfError(hr);

	axComPtr<ID3D12GraphicsCommandList>	cmdList;

	hr = _dxrDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(cmdList.ptrForInit()));
	throwIfError(hr);

	hr = cmdList->QueryInterface(IID_PPV_ARGS(_dxrCmdList.ptrForInit()));
	throwIfError(hr);

	_globalRootSignature.create(this);
	_localRootSignature.create(this);

	_createPipelineStateObject();

	// Allocate a heap for 3 descriptors:
	// 2 - bottom and top level acceleration structure fallback wrapped pointers
	// 1 - raytracing output texture SRV
	_descriptorHeap.create(3);
	{
		axStrLiteralW rayGenFunc[] = {L"MyRaygenShader"};
		_rayGenShaderTable.create(this, rayGenFunc, &_localRootSignature._rayGenParamData);
	}
	{
		axStrLiteralW missingFunc[] = {L"MyMissShader"};
		_missingShaderTable.create(this, missingFunc, nullptr);
	}
	{
		axStrLiteralW hitGroupFunc[] = {L"MyHitGroup"};
		_hitGroupShaderTable.create(this, hitGroupFunc, nullptr);
	}

	{
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::RayTracingScratch;
		_scratchBuffer.create(desc);
	}

	{
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::RayTracingInstanceDesc;
		_instanceDescsBuf.create(desc);
	}

	{
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::RayTracingAccelStruct;
		_bottomLevelAccelStruct.create(desc);
	}

	{
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::RayTracingAccelStruct;
		_topLevelAccelStruct.create(desc);
	}
	return true;
}

void axDX12RayTracing::GlobalRootSignature::create(axDX12RayTracing* rayTracing) {
	// Global Root Signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	D3D12_DESCRIPTOR_RANGE uav = {};
	uav.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	uav.NumDescriptors = 1;
	uav.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[ParamId::_Count] = {};
	{
		auto& rp = rootParameters[ParamId::OutputViewSlot];
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rp.DescriptorTable.NumDescriptorRanges = 1;
		rp.DescriptorTable.pDescriptorRanges = &uav;
	}

	{
		auto& rp = rootParameters[ParamId::AccelerationStructureSlot];
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = ARRAYSIZE(rootParameters);
	desc.pParameters   = rootParameters;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;

	rayTracing->_createRootSignature(_p, desc);
}

void axDX12RayTracing::LocalRootSignature::create(axDX12RayTracing* rayTracing) {
	// Local Root Signature
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	D3D12_ROOT_PARAMETER rootParameters[ParamId::_Count] = {};
	{
		auto& rp = rootParameters[ParamId::ViewportConstantSlot];
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rp.Constants.Num32BitValues = sizeofInUInt32(_rayGenParamData);
		rp.Constants.RegisterSpace = 0;
		rp.Constants.ShaderRegister = 0;
	}

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = ARRAYSIZE(rootParameters);
	desc.pParameters = rootParameters;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;

	rayTracing->_createRootSignature(_p, desc);
}

void axDX12RayTracing::_createRootSignature(axComPtr<ID3D12RootSignature>& outValue, const D3D12_ROOT_SIGNATURE_DESC& desc) {
	HRESULT hr;
	axComPtr<ID3DBlob> blob;
	axComPtr<ID3DBlob> error;
	hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, blob.ptrForInit(), error.ptrForInit());
	throwIfError(hr, error);

	hr = _dxrDevice->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(outValue.ptrForInit()));
	throwIfError(hr);
}

axDX12RayTracing::PipelineStateObject::PipelineStateObject() {
}

void axDX12RayTracing::PipelineStateObject::create(axDX12RayTracing* rayTracing) {
	_libDesc.NumExports = castUINT(_exportDescList.size());
	_libDesc.pExports = _exportDescList.data();

	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG,					&_stateObjectConfig);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE,					&_globalRootSignature);
	auto ri = _addSubObject(D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE,		&_localRootSignature);
//	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK,								nullptr);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY,							&_libDesc);
//	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION,					nullptr);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION,		&_exportsAssociation);
//	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION, nullptr);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG,				&_shaderConfig);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG,			&_pipelineConfig);
	_addSubObject(D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP,								&_hitGroupDesc);

//----
	_exportsAssociation.NumExports = castUINT(_exportsAssociationList.size());
	_exportsAssociation.pExports = _exportsAssociationList.data();
	_exportsAssociation.pSubobjectToAssociate = &_subObjects[ri]; // get subobject pointer only when subObjects size not longer change !!

	D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
	stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	stateObjectDesc.NumSubobjects = castUINT(_subObjects.size());
	stateObjectDesc.pSubobjects = _subObjects.data();

	HRESULT hr;

	hr = rayTracing->_dxrDevice->CreateStateObject(&stateObjectDesc, IID_PPV_ARGS(_stateObject.ptrForInit()));
	throwIfError(hr);

	hr = _stateObject->QueryInterface(IID_PPV_ARGS(_stateObjectProperties.ptrForInit()));
	throwIfError(hr);
}

void axDX12RayTracing::_createPipelineStateObject() {
	axTempString shaderFileName;
	shaderFileName.format("../../assets/shaders/outdir/raytracing/RayTracing.axShader/DX12_pass0.{?}.bin", axShaderStageProfile::DX12_Lib);
	_shaderByteCode.openFile(shaderFileName);

	_pipelineStateObject.setLibShaderByteCode(_shaderByteCode);

	_pipelineStateObject.defineLibExport(L"MyRaygenShader");
	_pipelineStateObject.defineLibExport(L"MyClosestHitShader");
	_pipelineStateObject.defineLibExport(L"MyMissShader");

	_pipelineStateObject.defineRootAssiciationExport(L"MyRaygenShader");

	_pipelineStateObject.setHitGroup_Type(D3D12_HIT_GROUP_TYPE_TRIANGLES);
	_pipelineStateObject.setHitGroup_ClosestHitShaderImport(L"MyClosestHitShader");
	_pipelineStateObject.setHitGroup_HitGroupExport(L"MyHitGroup");

	_pipelineStateObject.setShaderConfig_MaxPayloadSizeInBytes(4 * sizeof(float)); // float4 color
	_pipelineStateObject.setShaderConfig_MaxAttributeSizeInBytes(2 * sizeof(float)); // float2 barycentrics

	_pipelineStateObject.setGlobalRootSignature(_globalRootSignature._p);
	_pipelineStateObject.setLocalRootSignature(_localRootSignature._p);

	_pipelineStateObject.setpiplineConfig_MaxTraceRecursionDepth(1); // 1 = primary rays only

	_pipelineStateObject.create(this);
}

void axDX12RayTracing::ShaderTable::create(axDX12RayTracing* rayTracing, axSpan<axStrLiteralW> functions, const RayGenParamData* paramData) {
	axInt shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	auto* stateObjProps = rayTracing->_pipelineStateObject.stateObjectProperties();

	auto dataSize = shaderIdentifierSize * functions.size();
	if (paramData) {
		dataSize += ax_sizeof(RayGenParamData);
	}

	axByteArray	data;
	data.reserve(dataSize);

	for (auto& e : functions) {
		void* p = stateObjProps->GetShaderIdentifier(e.c_str());
		if (!p) {
			AX_LOG("cannot find shader function {?}", e);
			throw axError_Undefined(AX_LOC);
		}

		auto view = axByteSpan(reinterpret_cast<axByte*>(p), shaderIdentifierSize);
		data.appendRange(view);
	}

	if (paramData) {
		data.appendRange(axSpan_make(*paramData).toByteSpan());
	}

	_buf.create(axRenderBufferType::ShaderRecord, data);
}

#endif // AX_USE_DX12_DXR
