#if AX_USE_DX11

#include <ax_render/axRenderContext.h>
#include "axDX11CommonBase.h"
#include "axDX11Renderer.h"

axDX11Renderer* axDX11Util::renderer() {
	return axDX11Renderer::s_instance();
}

axDX11_ID3DDevice* axDX11Util::d3dDevice() {
	return axDX11Renderer::s_instance()->d3dDevice();
}

axDX11_ID3DDeviceContext* axDX11Util::d3dContext() {
	return axDX11Renderer::s_instance()->d3dContext();
}

#endif //#if AX_USE_DX11