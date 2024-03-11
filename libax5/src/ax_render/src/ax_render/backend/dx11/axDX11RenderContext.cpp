#if AX_USE_DX11

#include "../../axRenderContext.h"
#include "axDX11RenderContext.h"
#include "axDX11Renderer.h"
#include "axDX11CommandDispatcher.h"
#include "axDX11Fence.h"

const wchar_t* axDX11RenderContext::kClassName = L"axDX11RenderContext";

axDX11RenderContext::~axDX11RenderContext() {
	if (_hwnd) { ::DestroyWindow(_hwnd); _hwnd = nullptr; }
}

axDX11Renderer* axDX11RenderContext::renderer() {
	return static_cast<axDX11Renderer*>(_renderer);
}

axDX11RenderContext::axDX11RenderContext(axDX11Renderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
	_createWindow(desc);

	auto* d3dDevice = renderer->d3dDevice();
	auto* dxgiFactory = renderer->dxgiFactory();

	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width  = 8;
		swapChainDesc.BufferDesc.Height = 8;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = _hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		auto hr = dxgiFactory->CreateSwapChain(d3dDevice, &swapChainDesc, _swapChain.ptrForInit());
		Util::throwIfError(hr);
	}
}

LRESULT WINAPI axDX11RenderContext::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
		}break;

		case WM_DESTROY: {
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
		}break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			if (auto* thisObj = s_getThis(hwnd)) {
				if (thisObj->_eventHandler) {
					thisObj->_eventHandler->render(thisObj);
				}
			}
			EndPaint(hwnd, &ps);
			return 0;
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_window->_handleWin32Event(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void axDX11RenderContext::_createRenderTargetView() {
	auto* d3dDevice = renderer()->d3dDevice();

	axComPtr<ID3D11Texture2D> backBuffer;
	auto hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit()));
	Util::throwIfError(hr);

	_renderTargetView.unref();
	hr = d3dDevice->CreateRenderTargetView( backBuffer, nullptr, _renderTargetView.ptrForInit() );
	Util::throwIfError(hr);

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width  = backBufferDesc.Width;
	descDepth.Height = backBufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = d3dDevice->CreateTexture2D(&descDepth, nullptr, _depthStencil.ptrForInit());
	Util::throwIfError(hr);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = d3dDevice->CreateDepthStencilView(_depthStencil.ptr(), &descDSV, _depthStencilView.ptrForInit());
	Util::throwIfError(hr);
}

void axDX11RenderContext::_createWindow(CreateDesc& desc) {
	auto hInstance = ::GetModuleHandle(nullptr);
	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	// register window class
	WNDCLASSEX wc = {};
	bool registered = (0 != ::GetClassInfoEx(hInstance, kClassName, &wc));
	if (!registered) {
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= s_wndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= nullptr;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= kClassName;
		wc.hIconSm			= nullptr;
			
		if (!::RegisterClassEx(&wc)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	//--------
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								0, 0, 0, 0,
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		throw axError_Undefined(AX_LOC);
	}

	ShowWindow(_hwnd, SW_SHOW);
}

void axDX11RenderContext::cmdSwapBuffers() {
	auto hr = _swapChain->Present(_renderer->vsync() ? 1 : 0, 0);
	Util::throwIfError(hr);
}

void axDX11RenderContext::onSetNativeViewRect(const axRect2f& rect) {
	::MoveWindow(_hwnd, int(rect.x), int(rect.y), int(rect.w), int(rect.h), false);

	_renderTargetView.unref(); // release buffer and render target view before resize

	auto hr = _swapChain->ResizeBuffers(0, static_cast<UINT>(ax_max(0.0f,rect.w)), static_cast<UINT>(ax_max(0.0f, rect.h)), DXGI_FORMAT_UNKNOWN, 0);
	Util::throwIfError(hr);

	_createRenderTargetView();

	Base::onSetNativeViewRect(rect);
}

void axDX11RenderContext::onDispatchCommands(axRenderRequest& req) {
	auto* d3dContext = renderer()->d3dContext();
	d3dContext->ClearState();

	auto* target = _renderTargetView.ptr();
	d3dContext->OMSetRenderTargets(1, &target, _depthStencilView);

	d3dContext->RSSetState(nullptr);
	d3dContext->OMSetDepthStencilState(nullptr, 0);
	d3dContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

//-----------
	axDX11CommandDispatcher dispatcher(this, &req);
	dispatcher._renderTargetView = _renderTargetView;
	dispatcher._depthStencilView = _depthStencilView;

	dispatcher.dispatch();

}

void axDX11RenderContext::onSetNeedToRender() {
	if (_hwnd) {
		RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

#endif //AX_USE_DX11
