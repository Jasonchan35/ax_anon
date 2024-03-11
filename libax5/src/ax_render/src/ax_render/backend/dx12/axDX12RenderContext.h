#pragma once

#if AX_USE_DX12

#include "axDX12DescripterHeap.h"
#include "../../axRenderContext.h"

class axDX12RenderContext : public axRenderContext {
	AX_RTTI_CLASS(axDX12RenderContext, axRenderContext);
public:
	using Util = axDX12Util;

	~axDX12RenderContext();

	axDX12RenderContext(axDX12Renderer* renderer, CreateDesc& desc);

	axDX12Renderer* renderer();

	void cmdSwapBuffers(axDX12CommandDispatcher& dispatcher);

protected:
	virtual void onDispatchCommands(axRenderRequest& req) override;
	virtual void onPreDestroy();
	virtual void onSetNeedToRender() override;
	virtual void onSetNativeViewRect(const axRect2f& rect) override;

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}


private:
	void _createWindow(CreateDesc& desc);

	void _createRenderTargetView();
	void _releaseRenderTargetView();

	void _setFrameIndex(UINT i);

	static const UINT kFrameBufferCount = 2; // front and back buffer	
	static const wchar_t* kClassName;
	axThreadId _currentThreadId;
	axComPtr<ID3D12CommandQueue>			_cmdQueue;
	axComPtr<ID3D12CommandQueue>			_computeCmdQueue;

	axComPtr<IDXGISwapChain3>				_swapChain;

	axDX12DescripterHeap_RenderTarget		_renderTargetDescHeap;
	axDX12DescripterHeap_DepthStencilBuffer	_depthStencilDescHeap;

	struct FrameData {
		UINT index = 0xffff;
		axDX12DescriptorHandle				renderTarget;
		axDX12DescriptorHandle				depthStencilBuffer;

		axDX12Resource_RenderTarget			_renderTargetResource;
		axDX12Resource_DepthStencilBuffer	_depthStencilBufferResource;
	};

	axFixedArray<FrameData, kFrameBufferCount>	_frameArray;
	FrameData*	_frame = nullptr;
	UINT		_frameIndex = 0;

	HWND _hwnd = nullptr;
};

#endif //AX_USE_DX12
