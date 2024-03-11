#pragma once

//
//  axGLRenderContext_Win32.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axGLRenderContext_Base.h"

#if AX_USE_Win32Api
#if AX_USE_OPENGL

class axGLRenderContext_Win32 : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_Win32, axGLRenderContext_Base)
public:
	axGLRenderContext_Win32(axGLRenderer* renderer, CreateDesc& desc);
	virtual ~axGLRenderContext_Win32();

	struct NativeContext {
		HGLRC	rc = nullptr;
		HDC		dc = nullptr;
	};

	HWND	_hwnd = nullptr;

protected:
	virtual void cmdSwapBuffers() override;
	virtual void onMakeGLContextCurrent() override;
	virtual void onUnmakeGLContextCurrent() override;

	virtual bool onIsGLContextCurrent() override;
	
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;

private:
	class FalseContext;
	static const wchar_t* kClassName;

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	NativeContext _ctx;
	bool _lastVSync : 1;
};


#endif //AX_USE_OPENGL
#endif //AX_OS_WINDOWS
