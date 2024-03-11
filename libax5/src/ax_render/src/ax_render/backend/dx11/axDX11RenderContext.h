#pragma once

#include "../../ax_render_config.h"

#if AX_USE_DX11

#include <ax_render/axRenderContext.h>
#include "axDX11CommonBase.h"

class axDX11Renderer;
class axDX11RenderVertexBuffer;
class axDX11RenderIndexBuffer;
class axDX11Material;
class axDX11Shader;

class axDX11RenderContext : public axRenderContext {
	AX_RTTI_CLASS(axDX11RenderContext, axRenderContext);
public:
	using Util = axDX11Util;

	axDX11RenderContext(axDX11Renderer* renderer, CreateDesc& desc);
	~axDX11RenderContext();

	axDX11Renderer* renderer();

	HWND _hwnd = nullptr;

			void cmdSwapBuffers();

protected:
	virtual void onDispatchCommands(axRenderRequest& req) override;
	virtual void onSetNeedToRender() override;
	virtual void onSetNativeViewRect(const axRect2f& rect) override;

private:

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	void _createRenderTargetView();
	void _createWindow(CreateDesc& desc);

	static const wchar_t* kClassName;
	axComPtr<IDXGISwapChain>			_swapChain;
	axComPtr<ID3D11RenderTargetView>	_renderTargetView;
	axComPtr<ID3D11Texture2D>			_depthStencil;
	axComPtr<ID3D11DepthStencilView>	_depthStencilView;
};

#endif //AX_USE_DX11
