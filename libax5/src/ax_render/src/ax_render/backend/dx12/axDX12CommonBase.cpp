#if AX_USE_DX12

#include <ax_render/axRenderContext.h>
#include <ax_render/texture/axSamplerState.h>
#include "axDX12Resource.h"
#include "axDX12CommonBase.h"
#include "axDX12Renderer.h"

axDX12Renderer* axDX12Util::renderer() {
	return axDX12Renderer::s_instance();
}

axDX12_ID3D12Device* axDX12Util::d3dDevice() {
	return axDX12Renderer::s_instance()->d3dDevice();
}

void axDX12Util::reportError(HRESULT hr) {
#if 0 && _DEBUG
	auto* d = renderer()->dxgiDebug();
	if (d) {
		d->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
#endif
	AX_ASSERT(false);
}

#endif //#if AX_USE_DX12
