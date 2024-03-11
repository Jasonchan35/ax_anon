#include "axDX12RenderContext.h"

#if AX_USE_DX12

#include "axDX12Renderer.h"
#include "axDX12CommandDispatcher.h"
#include "axDX12Fence.h"

const wchar_t* axDX12RenderContext::kClassName = L"axDX12RenderContext";

axDX12RenderContext::axDX12RenderContext(axDX12Renderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	_createWindow(desc);

	auto* d3dDevice = renderer->d3dDevice();
	auto* dxgiFactory = renderer->dxgiFactory();

	HRESULT hr;

	{	// create command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE; // D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
		queueDesc.NodeMask	= 0;

		hr = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_cmdQueue.ptrForInit()));
		Util::throwIfError(hr);

		queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_computeCmdQueue.ptrForInit()));
		Util::throwIfError(hr);
	}

	{	// create swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width  = 8;
		swapChainDesc.Height = 8;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = kFrameBufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING <-- for vsync ?

		axComPtr<IDXGISwapChain1> swapChain1;
		hr = dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, _hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.ptrForInit());
		Util::throwIfError(hr);

		hr = swapChain1->QueryInterface(IID_PPV_ARGS(_swapChain.ptrForInit()));
		Util::throwIfError(hr);

		_setFrameIndex(_swapChain->GetCurrentBackBufferIndex());
	}

	_createRenderTargetView();
}

axDX12Renderer* axDX12RenderContext::renderer() {
	return static_cast<axDX12Renderer*>(_renderer);
}

void axDX12RenderContext::_createRenderTargetView() {
	_releaseRenderTargetView();

	_renderTargetDescHeap.init(kFrameBufferCount);
	_depthStencilDescHeap.init(kFrameBufferCount);

	// Create a RTV for each frame.
	for (UINT i = 0; i < kFrameBufferCount; i++) {
		auto* frame = &_frameArray[i];
		frame->index = _swapChain->GetCurrentBackBufferIndex();

		frame->_renderTargetResource.createFromSwapChain(_swapChain, i);

		DXGI_SWAP_CHAIN_DESC desc;
		auto hr = _swapChain->GetDesc(&desc);
		Util::throwIfError(hr);

		axVec2i bufferSize(desc.BufferDesc.Width, desc.BufferDesc.Height);
		frame->_depthStencilBufferResource.create(bufferSize);

		frame->renderTarget       = _renderTargetDescHeap.createView(i, frame->_renderTargetResource);
		frame->depthStencilBuffer = _depthStencilDescHeap.createView(i, frame->_depthStencilBufferResource);
	}
}

void axDX12RenderContext::_releaseRenderTargetView() {
	for (UINT i = 0; i < kFrameBufferCount; i++) {
		auto* f = &_frameArray[i];
		f->_renderTargetResource.destroy();
		f->_depthStencilBufferResource.destroy();
	}
}


void axDX12RenderContext::cmdSwapBuffers(axDX12CommandDispatcher& dispatcher) {
	{	// Indicate that the back buffer will now be used to present.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = _frame->_renderTargetResource.d3dResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}
	dispatcher._execCmdList();

	{
		auto* fence = ax_type_cast_debug<axDX12Fence>(dispatcher.renderRequest->completedFence());
		fence->addToGpu(dispatcher.cmdQueue);
	}

	auto hr = _swapChain->Present(_renderer->vsync() ? 1 : 0, 0);
	Util::throwIfError(hr);
}

void axDX12RenderContext::onDispatchCommands(axRenderRequest& req_) {
	_setFrameIndex(_swapChain->GetCurrentBackBufferIndex());

	auto* req = ax_type_cast_debug<axDX12RenderRequest>(&req_);

	axDX12CommandDispatcher dispatcher(this, req);
	dispatcher.cmdQueue.ref(_cmdQueue);
	dispatcher.computeCmdQueue.ref(_computeCmdQueue);
	dispatcher.renderTarget = _frame->renderTarget;
	dispatcher.depthStencilBuffer = _frame->depthStencilBuffer;

	// Set necessary state.
	//_cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
	//_cmdList->RSSetViewports(1, &m_viewport);
	//_cmdList->RSSetScissorRects(1, &m_scissorRect);

	{	// Indicate that the back buffer will be used as a render target.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = _frame->_renderTargetResource.d3dResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}
	dispatcher.cmdList->OMSetRenderTargets(1, &_frame->renderTarget.cpu, FALSE, &_frame->depthStencilBuffer.cpu);


#if AX_USE_DX12_DXR
	dispatcher.rayTracing = ax_type_cast_debug<axDX12RayTracing>(renderer()->rayTracing());
#endif


	dispatcher.dispatch();
}

void axDX12RenderContext::onPreDestroy() {
}

void axDX12RenderContext::onSetNeedToRender() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

axDX12RenderContext::~axDX12RenderContext() {
}

void axDX12RenderContext::onSetNativeViewRect(const axRect2f& rect) {
	::MoveWindow(_hwnd, int(rect.x), int(rect.y), int(rect.w), int(rect.h), false);

	_releaseRenderTargetView();

	auto width  = static_cast<UINT>(ax_max(8.0f, rect.w));
	auto height = static_cast<UINT>(ax_max(8.0f, rect.h));
	auto hr = _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	Util::throwIfError(hr);

	_createRenderTargetView();

	Base::onSetNativeViewRect(rect);
}

LRESULT WINAPI axDX12RenderContext::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

void axDX12RenderContext::_createWindow(CreateDesc& desc) {
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

void axDX12RenderContext::_setFrameIndex(UINT i) {
	_frameIndex = i;
	_frame = &_frameArray[i];
}

#endif //AX_USE_DX12
