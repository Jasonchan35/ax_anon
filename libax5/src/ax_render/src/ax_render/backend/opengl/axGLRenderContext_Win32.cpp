//
//  axGLRenderContext_Win32.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axGLRenderContext.h"
#include "axGLRenderer.h"
#include "axGLCommonBase.h"
#include "axGLRenderContext_Win32.h"

#if AX_USE_Win32Api
#if AX_USE_OPENGL

const wchar_t* axGLRenderContext_Win32::kClassName = L"axGLRenderContext";

// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)#Create_a_False_Context
// The key problem is this: the function you use to get WGL extensions is, itself, an OpenGL extension
class axGLRenderContext_Win32::FalseContext : public axNonCopyable {
public:
	FalseContext() {
	}

	~FalseContext() {
		if (_rc)   { ::wglDeleteContext(_rc);	_rc   = nullptr; }
		if (_dc)   { ::ReleaseDC(_hwnd, _dc);	_dc   = nullptr; }
		if (_hwnd) { ::DestroyWindow(_hwnd);	_hwnd = nullptr; }		
	}

	void create() {
		auto hInstance = ::GetModuleHandle(nullptr);

		_hwnd = ::CreateWindowExW(	0, 
									kClassName, 
									L"",
									WS_POPUP,
									0, 0, 0, 0,
									nullptr, nullptr, hInstance, this);
		if (!_hwnd) {
			throw axGLError_Undefined();
		}

		_dc = GetDC(_hwnd);
		if (!_dc) {
			throw axGLError_Undefined();
		}

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize		= sizeof(pfd);
		pfd.nVersion	= 1;
		pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType	= PFD_TYPE_RGBA;
		pfd.cColorBits	= 32;
		pfd.cDepthBits	= 32;
		pfd.iLayerType	= PFD_MAIN_PLANE;
		
		int nPixelFormat = ::ChoosePixelFormat(_dc, &pfd);
		if (nPixelFormat == 0) {
			throw axGLError_Undefined();
		}
		
		BOOL bResult = ::SetPixelFormat(_dc, nPixelFormat, &pfd);
		if (!bResult) {
			throw axGLError_Undefined();
		}

		_rc = ::wglCreateContext(_dc);
		if (!_rc) {
			throw axGLError_Undefined();
		}

		::wglMakeCurrent(_dc, _rc);
	}

private:
	HWND	_hwnd = nullptr;
	HDC		_dc = nullptr;
	HGLRC	_rc = nullptr;
};

axGLRenderContext_Win32::axGLRenderContext_Win32(axGLRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
	, _lastVSync(true)
{
	auto hInstance = ::GetModuleHandle(nullptr);

	FalseContext falseContext;

	if (!desc.window) { //root context

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

		// create False context in order to init glew to have wglCreateContextAttribsARB()
		falseContext.create();
		glewInit();
		
		//axGLUtil::logExtensions();
		Util::checkGLError(AX_LOC);
	}

	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	// create real context
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								0, 0, 0, 0,
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		throw axError_Undefined(AX_LOC);
	}
		
	_ctx.dc = GetDC(_hwnd);
	if (!_ctx.dc) {
		throw axError_Undefined(AX_LOC);
	}

// create context
	const int formatAttrs[] = {
		WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE, 
		WGL_SUPPORT_OPENGL_ARB,	GL_TRUE, 
		WGL_DOUBLE_BUFFER_ARB,	GL_TRUE, 
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB, 
		WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,		32,
		WGL_ALPHA_BITS_ARB,		8,
		WGL_DEPTH_BITS_ARB,		24,
		WGL_STENCIL_BITS_ARB,	8,
		//WGL_SAMPLE_BUFFERS_ARB,	GL_TRUE,
		//WGL_SAMPLES_ARB,		4,
		0 // End of attributes list
	}; 

	int format, numFormat; 
	if (!wglChoosePixelFormatARB(_ctx.dc, formatAttrs, nullptr, 1, &format, (UINT*)&numFormat)) {
		throw axError_Undefined(AX_LOC);
	}

	PIXELFORMATDESCRIPTOR pfd;
	if (!::DescribePixelFormat(_ctx.dc, format, sizeof(pfd), &pfd)) {
		throw axError_Undefined(AX_LOC);
	}	

	if(!::SetPixelFormat(_ctx.dc, format, &pfd)) {
		throw axError_Undefined(AX_LOC);
	}

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	int contextAttrs[] = { 
		WGL_CONTEXT_MAJOR_VERSION_ARB, major, // 3
		WGL_CONTEXT_MINOR_VERSION_ARB, minor, // 3
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
		0 // End of attributes list
	}; 

	{
		axGLRenderer::ScopedRootContext scopedRootContext;

		HGLRC sharedContext = nullptr;
		if (auto rootContext = scopedRootContext.rootContext()) {
			sharedContext = rootContext->_ctx.rc;
		}

		_ctx.rc = wglCreateContextAttribsARB(_ctx.dc, sharedContext, contextAttrs);
		if (!_ctx.rc) {
			Util::checkGLError(AX_LOC);
			throw axError_Undefined(AX_LOC);
		}
	}

	ScopedContext scopedContext(this);

	if (desc.window) {
		::ShowWindow(_hwnd, SW_SHOW);
	}
	Util::checkGLError(AX_LOC);
}
	
axGLRenderContext_Win32::~axGLRenderContext_Win32() {
	onPreDestroy();

	if (_ctx.rc)	{ ::wglDeleteContext(_ctx.rc);	_ctx.rc = nullptr; }
	if (_ctx.dc)	{ ::ReleaseDC(_hwnd, _ctx.dc);	_ctx.dc = nullptr; }
	if (_hwnd)		{ ::DestroyWindow(_hwnd);		_hwnd   = nullptr; }
}
	
LRESULT WINAPI axGLRenderContext_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

void axGLRenderContext_Win32::cmdSwapBuffers() {
	auto vsync = _renderer->vsync();
	if (_lastVSync != vsync) {
		wglSwapIntervalEXT(vsync ? 1 : 0);
		_lastVSync = vsync;
	}
	::SwapBuffers(_ctx.dc);
}
	
void axGLRenderContext_Win32::onMakeGLContextCurrent() {
	::wglMakeCurrent(_ctx.dc, _ctx.rc);
}

void axGLRenderContext_Win32::onUnmakeGLContextCurrent() {
	::wglMakeCurrent(nullptr, nullptr);
}
	
bool axGLRenderContext_Win32::onIsGLContextCurrent() {
	if (!_ctx.rc) return false;
	return _ctx.rc == ::wglGetCurrentContext();
}
	
void axGLRenderContext_Win32::onSetNativeViewRect(const axRect2f& rect) {
	::MoveWindow(_hwnd, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, true);
	Base::onSetNativeViewRect(rect);
}
	
void axGLRenderContext_Win32::onSetNeedToRender() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

#endif //AX_USE_OPENGL
#endif // AX_OS_WINDOWS
