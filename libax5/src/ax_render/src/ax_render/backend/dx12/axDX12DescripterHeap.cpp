#if AX_USE_DX12

#include <ax_render/texture/axSamplerState.h>
#include "axDX12DescripterHeap.h"
#include "axDX12RenderContext.h"

void axDX12DescripterHeap_Base::destroy() {
	_d3dHeap.unref();
	_numDescriptors = 0;
	_heapStartHandle = axDX12DescriptorHandle();
}

void axDX12DescripterHeap_Base::_init(axInt numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	if (numDescriptors <= 0) {
		destroy();
		return;
	}

	if (_d3dHeap && _desc.Type == type && _desc.Flags == flags && _numDescriptors == numDescriptors)
		return;

	_renderer = Util::renderer();

	auto* d3dDevice = _renderer->d3dDevice();
	_stride = d3dDevice->GetDescriptorHandleIncrementSize(type);

	ax_safe_assign(_desc.NumDescriptors, numDescriptors);
	_desc.Type  = type;
	_desc.Flags = flags;
	auto hr = d3dDevice->CreateDescriptorHeap(&_desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Util::throwIfError(hr);

	_heapStartHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	_heapStartHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
	_numDescriptors = numDescriptors;
}

#endif
