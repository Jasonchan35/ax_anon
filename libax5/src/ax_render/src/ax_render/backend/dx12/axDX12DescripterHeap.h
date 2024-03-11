#pragma once

#if AX_USE_DX12

#include "axDX12Renderer.h"
#include "axDX12Resource.h"

class axDX12Renderer;

struct axDX12DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};

	operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return cpu; }
	operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return gpu; }
};

class axDX12DescripterHeap_Base : public axNonCopyable {
public:
	using Util = axDX12Util;

	ID3D12DescriptorHeap*	d3dHeap() { return _d3dHeap; }

	bool isValid() const { return _d3dHeap.ptr() != nullptr; }
	void destroy();

	axDX12DescriptorHandle heapStartHandle() {
		return _heapStartHandle;
	}

	axDX12DescriptorHandle elementHandle(axInt i) {
		if (i < 0 || i >= _numDescriptors)
			throw axError_Undefined(AX_LOC);

		auto o = _heapStartHandle;
		auto offset = i * _stride;
		o.cpu.ptr += offset;
		o.gpu.ptr += offset;
		return o;
	}

protected:
	void _init(axInt numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	axComPtr<ID3D12DescriptorHeap>	_d3dHeap;
	axDX12Renderer*					_renderer = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	axInt							_numDescriptors = 0;
	axDX12DescriptorHandle			_heapStartHandle;
	UINT							_stride = 0;
};

class axDX12DescripterHeap_RenderTarget : public axDX12DescripterHeap_Base {
public:
	void init(axInt numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	axDX12DescriptorHandle createView(axInt i, axDX12Resource_RenderTarget& res) {
		//	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateRenderTargetView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class axDX12DescripterHeap_DepthStencilBuffer : public axDX12DescripterHeap_Base {
public:
	void init(axInt numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	axDX12DescriptorHandle createView(axInt i, axDX12Resource_DepthStencilBuffer& res) {
		//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateDepthStencilView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class axDX12DescripterHeap_CBV_SRV_UAV : public axDX12DescripterHeap_Base {
public:
	void init(axInt numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	axDX12DescriptorHandle createViewCBV(axInt i, axDX12Resource_Buffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = res.gpuAddress();
		desc.SizeInBytes    = Util::castUINT(res.alignedDataSize());

		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateConstantBufferView(&desc, h);
		return h;
	}

	axDX12DescriptorHandle createViewStructuredUAV(axInt i, axDX12Resource_Buffer& buf) {
		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, nullptr, h);
		return h;
	}

	axDX12DescriptorHandle createViewRawUAV(axInt i, axDX12Resource_Buffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Util::castUINT(buf.dataSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, &desc, h);
		return h;
	}

	axDX12DescriptorHandle createViewSRV(axInt i, axDX12Resource_Texture& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		_renderer->d3dDevice()->CreateShaderResourceView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class axDX12DescripterHeap_Sampler : public axDX12DescripterHeap_Base {
public:
	void init(axInt numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	axDX12DescriptorHandle createSampler(axInt i, const D3D12_SAMPLER_DESC& desc) {
		auto o = elementHandle(i);
		_renderer->d3dDevice()->CreateSampler(&desc, o.cpu);
		return o;
	}
};

#endif