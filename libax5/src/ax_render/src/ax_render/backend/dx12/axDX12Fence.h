#pragma once

#include "../../axRenderFence.h"

class axDX12Fence : public axRenderFence {
	AX_RTTI_CLASS(axDX12Fence, axRenderFence)
public:
	using Util = axDX12Util;

	axDX12Fence() {
		auto hr = Util::d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE /*| D3D12_FENCE_FLAG_SHARED*/, IID_PPV_ARGS(_fence.ptrForInit()));
		Util::throwIfError(hr);
	}

	void addToGpu(ID3D12CommandQueue* cmdQueue) {
		cmdQueue->Signal(_fence, 1);
	}

	bool onCheckCompleted() override {
		if (!_fence) throw axError_Undefined(AX_LOC);
		auto v = _fence->GetCompletedValue();
		if (v) return true;

		return false;
	}

	axComPtr<ID3D12Fence> _fence;
};