//
//  axVkRenderContext_Win32.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axVkRenderContext.h"
#include "axVkRenderer.h"
#include "axVkCommonBase.h"
#include "axVkRenderContext_Win32.h"

#if AX_USE_Win32Api
#if AX_USE_VULKAN

const wchar_t* axVkRenderContext_Win32::kClassName = L"axVkRenderContext";

axVkRenderContext_Win32::axVkRenderContext_Win32(axVkRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
	auto hInstance = ::GetModuleHandle(nullptr);

	// register window class
	WNDCLASSEX wc = {};
	bool registered = (0 != ::GetClassInfoEx(hInstance, kClassName, &wc));
	if (!registered) {
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= s_wndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= nullptr;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= nullptr; // (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= kClassName;
		wc.hIconSm			= nullptr;
			
		if (!::RegisterClassEx(&wc)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	// create real context
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								0, 0, 8, 8,
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		AX_LOG_WIN32_LAST_ERROR("CreateWindow");
		throw axError_Undefined(AX_LOC);
	}

	_createDevice();

	if (desc.window) {
		::ShowWindow(_hwnd, SW_SHOW);
	}
}
	
void axVkRenderContext_Win32::onCreateVkSurface(axCppVkSurfaceKHR& surface) {
	auto hInstance = ::GetModuleHandle(nullptr);
	auto& device = renderer()->device();
	surface.create_Win32(*device.physicalDevice(), hInstance, _hwnd);
}

axVkRenderContext_Win32::~axVkRenderContext_Win32() {
	onPreDestroy();

	if (_hwnd   ) { ::DestroyWindow(_hwnd);	_hwnd = nullptr; }
}
	
LRESULT WINAPI axVkRenderContext_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_window->_handleWin32Event(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void axVkRenderContext_Win32::onSetNativeViewRect(const axRect2f& rect) {
	::MoveWindow(_hwnd, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, true);
	Base::onSetNativeViewRect(rect);
}
	
void axVkRenderContext_Win32::onSetNeedToRender() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

#endif //AX_USE_VULKAN
#endif // AX_OS_WINDOWS
