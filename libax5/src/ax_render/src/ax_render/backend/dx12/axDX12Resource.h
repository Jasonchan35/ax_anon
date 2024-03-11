#pragma once

#include "../../ax_render_config.h"

#if AX_USE_DX12

#include "axDX12CommonBase.h"

class axDX12Resource_Base : public axNonCopyable {
public:
	using Util = axDX12Util;

	void destroy();

	void uploadToGpu(axInt offset, axByteSpan data);

	ID3D12Resource*	d3dResource() { return _d3dResource; }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _d3dResource ? _d3dResource->GetGPUVirtualAddress() : 0; }

	axInt dataSize() const { return _dataSize; }
	axInt alignedDataSize() const { return _alignedDataSize; }

	ID3D12Resource** ptrForInit() { return _d3dResource.ptrForInit(); }

	bool isValid() const { return _d3dResource.ptr() != nullptr; }

	D3D12_RESOURCE_STATES	resourceState() const { return _resourceState; }

	void resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state);


protected:
	axDX12Resource_Base();
	void _create(const D3D12_CLEAR_VALUE* clearValue = nullptr);

	axComPtr<ID3D12Resource> _d3dResource;
	axInt _dataSize = 0;
	axInt _alignedDataSize = 0;

	D3D12_RESOURCE_DESC		_desc  = {};
	D3D12_HEAP_PROPERTIES	_heapProps = {};
	D3D12_RESOURCE_STATES	_resourceState;
};

class axDX12Resource_RenderTarget : public axDX12Resource_Base {
public:
	void createFromSwapChain(IDXGISwapChain3* swapChain, UINT i);
};

class axDX12Resource_DepthStencilBuffer : public axDX12Resource_Base {
public:
	void create(axVec2i size);
};

class axDX12Resource_Buffer : public axDX12Resource_Base {
public:
	void create(axRenderBufferType type, axInt bufferSize);
	void create(axRenderBufferType type, axByteSpan data) {
		create(type, data.sizeInBytes());
		uploadToGpu(0, data);
	}
};

class axDX12Resource_Texture : public axDX12Resource_Base {
};

class axDX12Resource_Texture2D : public axDX12Resource_Texture {
public:
	void create(axVec2i size, axInt mipmapCount, axColorType colorType);
};

#endif //AX_USE_DX12