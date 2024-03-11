#pragma once

#if AX_USE_DX12_DXR

#include "axDX12CommonBase.h"
#include "axDX12DescripterHeap.h"
#include "axDX12Resource.h"

class axDX12RayTracing : public axRenderRayTracing {
	AX_RTTI_CLASS(axDX12RayTracing, axRenderRayTracing);
public:
	using Util = axDX12Util;

	bool create();

	ID3D12Device5*	dxrDevice	()	{ return _dxrDevice; }
	ID3D12GraphicsCommandList4* dxrCmdList() { return _dxrCmdList; }
	ID3D12CommandQueue*	cmdQueue() { return _cmdQueue; }

	struct RayGenViewport {
		float left;
		float top;
		float right;
		float bottom;
	};

	struct RayGenParamData {
		RayGenViewport viewport;
		RayGenViewport stencil;
	};

	class GlobalRootSignature : public axNonCopyable {
	public:
		enum ParamId {
			OutputViewSlot,
			AccelerationStructureSlot,
			_Count
		};
		void create(axDX12RayTracing* rayTracing);

		axComPtr<ID3D12RootSignature>	_p;
	};

	class LocalRootSignature : public axNonCopyable {
	public:
		enum ParamId {
			ViewportConstantSlot,
			_Count 
		};
		void create(axDX12RayTracing* rayTracing);

		template<class T>
		UINT sizeofInUInt32(const T& obj) { return (sizeof(obj) - 1) / sizeof(axUInt32) + 1; }

		axComPtr<ID3D12RootSignature>	_p;
		RayGenParamData		_rayGenParamData;
	};

	class PipelineStateObject : public axNonCopyable, axDX12CommonBase {
	public:
		PipelineStateObject();

		void setShaderConfig_MaxPayloadSizeInBytes		(UINT v)		{ _shaderConfig.MaxPayloadSizeInBytes    = v; }
		void setShaderConfig_MaxAttributeSizeInBytes	(UINT v)		{ _shaderConfig.MaxAttributeSizeInBytes  = v; }
		void setpiplineConfig_MaxTraceRecursionDepth	(UINT v)		{ _pipelineConfig.MaxTraceRecursionDepth = v; }
		void setStateObjectConfig	(D3D12_STATE_OBJECT_FLAGS flags)	{ _stateObjectConfig.Flags = flags; }

		void setGlobalRootSignature (ID3D12RootSignature* p)			{ _globalRootSignature.pGlobalRootSignature = p; };
		void setLocalRootSignature  (ID3D12RootSignature* p)			{ _localRootSignature.pLocalRootSignature   = p; };

		void setHitGroup_Type		(D3D12_HIT_GROUP_TYPE type)			{ _hitGroupDesc.Type = type; }
		void setHitGroup_AnyHitShaderImport			(LPCWSTR name)		{ _hitGroupDesc.AnyHitShaderImport			= name; }
		void setHitGroup_ClosestHitShaderImport		(LPCWSTR name)		{ _hitGroupDesc.ClosestHitShaderImport		= name; }
		void setHitGroup_IntersectionShaderImport	(LPCWSTR name)		{ _hitGroupDesc.IntersectionShaderImport	= name; }
		void setHitGroup_HitGroupExport				(LPCWSTR name)		{ _hitGroupDesc.HitGroupExport				= name; }

		void defineLibExport(LPCWSTR name, LPCWSTR exportToRename = nullptr, D3D12_EXPORT_FLAGS flags = D3D12_EXPORT_FLAG_NONE) {
			auto& s = _exportDescList.emplaceBack();
			s.Name = name;
			s.ExportToRename = exportToRename;
			s.Flags = flags;
		}

		void defineRootAssiciationExport(LPCWSTR name) {
			_exportsAssociationList.emplaceBack(name);
		}

		void setLibShaderByteCode(axByteSpan byteCode) {
			_libDesc.DXILLibrary.pShaderBytecode = byteCode.data();
			_libDesc.DXILLibrary.BytecodeLength = castUINT(byteCode.sizeInBytes());
		}

		void create(axDX12RayTracing* rayTracing);

		ID3D12StateObject* stateObject() { return _stateObject; }
		ID3D12StateObjectProperties* stateObjectProperties() { return _stateObjectProperties; }

	private:
		axInt _addSubObject(D3D12_STATE_SUBOBJECT_TYPE type, const void* p) {
			auto index = _subObjects.size();
			auto& s = _subObjects.emplaceBack();
			s.Type = type;
			s.pDesc = p;
			return index;
		}

		axArray<D3D12_EXPORT_DESC, 32 >			_exportDescList;
		axArray<LPCWSTR, 32>					_exportsAssociationList;

	//----- sub objects -------
		D3D12_STATE_OBJECT_CONFIG				_stateObjectConfig = {};
		D3D12_GLOBAL_ROOT_SIGNATURE				_globalRootSignature;
		D3D12_LOCAL_ROOT_SIGNATURE				_localRootSignature;
		D3D12_DXIL_LIBRARY_DESC					_libDesc = {};
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION	_exportsAssociation = {};
		D3D12_RAYTRACING_SHADER_CONFIG			_shaderConfig = {};
		D3D12_RAYTRACING_PIPELINE_CONFIG		_pipelineConfig = {};
		D3D12_HIT_GROUP_DESC					_hitGroupDesc = {};
	//-----
		axArray<D3D12_STATE_SUBOBJECT, 32>		_subObjects;
		axComPtr<ID3D12StateObject>				_stateObject;
		axComPtr<ID3D12StateObjectProperties>	_stateObjectProperties;
	};

	struct PointerWithSize {
		void*	ptr;
		UINT	size;
	};

	class ShaderTable {
	public:
		void create(axDX12RayTracing* rayTracing, axSpan<axStrLiteralW> functions, const RayGenParamData* paramData);

		axDX12Resource_Buffer	_buf;
	};

	ShaderTable			_rayGenShaderTable;
	ShaderTable			_missingShaderTable;
	ShaderTable			_hitGroupShaderTable;

	GlobalRootSignature	_globalRootSignature;
	LocalRootSignature	_localRootSignature;

	void _createRootSignature(axComPtr<ID3D12RootSignature>& outValue, const D3D12_ROOT_SIGNATURE_DESC& desc);

	void _createPipelineStateObject();

	axRenderMultiGpuBuffer	_scratchBuffer;
	axRenderMultiGpuBuffer	_instanceDescsBuf;
	axRenderMultiGpuBuffer	_bottomLevelAccelStruct;
	axRenderMultiGpuBuffer	_topLevelAccelStruct;

private:
	axMemMap				_shaderByteCode;
	axDX12Renderer*			_renderer = nullptr;

	axComPtr<ID3D12Device5>					_dxrDevice;
	axComPtr<ID3D12CommandQueue>			_cmdQueue;
	axComPtr<ID3D12CommandAllocator>		_cmdAllocator;
	axComPtr<ID3D12GraphicsCommandList4>	_dxrCmdList;

	axDX12DescripterHeap_CBV_SRV_UAV		_descriptorHeap;
	PipelineStateObject						_pipelineStateObject;
};

#endif