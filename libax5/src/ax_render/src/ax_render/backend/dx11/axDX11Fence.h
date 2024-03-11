#pragma once

#include "../../axRenderRequest.h"
#include "axDX11CommonBase.h"

class axDX11Fence : public axRenderFence {
	AX_RTTI_CLASS(axDX11Fence, axRenderFence)
public:
	using Util = axDX11Util;

	axDX11Fence() {
		D3D11_QUERY_DESC queryDesc = {};
		queryDesc.Query = D3D11_QUERY_EVENT;
		auto hr = Util::d3dDevice()->CreateQuery(&queryDesc, _query.ptrForInit());
		Util::throwIfError(hr);
	}

	void addToGpu(ID3D11DeviceContext* d3dContext) {
		d3dContext->End(_query);
		_addedToGpu = 1;
	}

	bool onCheckCompleted() override {
		if (!_addedToGpu) return false;

		BOOL data = false;
		auto hr = Util::d3dContext()->GetData(_query, &data, sizeof(data), 0);
		if (hr == S_FALSE) return false;
		if (hr == S_OK) {
			if (data) return true;
		}

		Util::throwIfError(hr);
		return false;
	}

	axAtomicInt8 _addedToGpu {0};
	axComPtr<ID3D11Query> _query;
};