#include "axDX12Resource.h"

#if AX_USE_DX12

axDX12Resource_Base::axDX12Resource_Base() {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	_desc.Alignment = 0;
	_desc.Width = 0;
	_desc.Height = 1;
	_desc.DepthOrArraySize = 1;
	_desc.MipLevels = 1;
	_desc.Format = DXGI_FORMAT_UNKNOWN;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	_heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	_heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	_heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	_heapProps.CreationNodeMask = 1;
	_heapProps.VisibleNodeMask = 1;

	_resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
}

void axDX12Resource_Base::_create(const D3D12_CLEAR_VALUE* clearValue) {
	auto hr = Util::d3dDevice()->CreateCommittedResource(
		&_heapProps,
		D3D12_HEAP_FLAG_NONE,
		&_desc,
		_resourceState,
		clearValue,
		IID_PPV_ARGS(_d3dResource.ptrForInit()));
	Util::throwIfError(hr);
}

void axDX12Resource_Base::destroy() {
	_d3dResource.unref();
	_dataSize = 0;
	_alignedDataSize = 0;
}

void axDX12Resource_Buffer::create(axRenderBufferType type, axInt bufferSize) {
	using Type = axRenderBufferType;

	destroy();

	_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	axInt alignment = 0;
	switch (type) {
		case Type::Vertex: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case Type::Index: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case Type::Const: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case Type::Upload: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case Type::Stage: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			_resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
			_heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		}break;

		case Type::Storage: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			// _resourceState	= D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			_resourceState	= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE; // | D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
//			_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		case Type::ShaderRecord: {
			alignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
		}break;

		case Type::RayTracingInstanceDesc: {
			alignment = D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT;
		}break;

		case Type::RayTracingScratch: {
			_resourceState	= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		case Type::RayTracingAccelStruct: {
			_resourceState	= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		default: throw axError_Undefined(AX_LOC);
	}

	if (bufferSize <= 0) {
		return;
	}

	_alignedDataSize = bufferSize;
	if (alignment > 0) {
		_alignedDataSize = ax_multiple_of(_alignedDataSize, alignment);
	}
	_desc.Width = Util::castUINT(_alignedDataSize);

	_create();

	_dataSize = bufferSize;
}

void axDX12Resource_Base::uploadToGpu(axInt offset, axByteSpan data) {
	D3D12_RANGE readRange = {}; // We do not intend to read from this resource on the CPU.
	UINT8* dst = nullptr;

	auto hr = _d3dResource->Map(0, &readRange, reinterpret_cast<void**>(&dst));
	Util::throwIfError(hr);

	if (offset < 0 || offset + data.size() > _dataSize)
		throw axError_Undefined(AX_LOC);

	ax_memcpy(dst + offset, data.data(), data.sizeInBytes());
	_d3dResource->Unmap(0, nullptr);
}

void axDX12Resource_RenderTarget::createFromSwapChain(IDXGISwapChain3* swapChain, UINT i) {
	auto hr = swapChain->GetBuffer(i, IID_PPV_ARGS(_d3dResource.ptrForInit()));
	Util::throwIfError(hr);
	_desc = _d3dResource->GetDesc();

	D3D12_HEAP_FLAGS flags;
	_d3dResource->GetHeapProperties(&_heapProps, &flags);
}

void axDX12Resource_Base::resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state) {
	if (!_d3dResource) throw axError_Undefined(AX_LOC);
	if (_resourceState & state) return; // only change when does not have specify bit

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = _d3dResource;
	barrier.Transition.StateBefore = _resourceState;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(1, &barrier);

	_resourceState = state;
}

void axDX12Resource_DepthStencilBuffer::create(axVec2i size) {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_desc.MipLevels = 0;
	_desc.Format = DXGI_FORMAT_D32_FLOAT;
	_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	_resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	_desc.Width  = Util::castUINT(size.x);
	_desc.Height = Util::castUINT(size.y);

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = _desc.Format;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	_create(&clearValue);
}

void axDX12Resource_Texture2D::create(axVec2i size, axInt mipmapCount, axColorType colorType) {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_desc.Format = Util::getDxColorType(colorType);
	_resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	_desc.Width     = Util::castUINT(size.x);
	_desc.Height    = Util::castUINT(size.y);
	_desc.MipLevels = Util::castUINT16(mipmapCount);

	_create();
}

#endif //#if AX_USE_DX12
