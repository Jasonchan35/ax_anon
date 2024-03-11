#pragma once

//
//  axVkRenderContext_Win32.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axVkRenderContext_Base.h"

#if AX_USE_Win32Api
#if AX_USE_VULKAN

class axVkRenderContext_Win32 : public axVkRenderContext_Base {
	AX_RTTI_CLASS(axVkRenderContext_Win32, axVkRenderContext_Base)
public:
	axVkRenderContext_Win32(axVkRenderer* renderer, CreateDesc& desc);
	virtual ~axVkRenderContext_Win32();
	
	HWND	_hwnd = nullptr;

protected:
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	virtual void onCreateVkSurface(axCppVkSurfaceKHR& surface) override;

private:
	static const wchar_t* kClassName;

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
};


#endif //AX_USE_VULKAN
#endif //AX_OS_WINDOWS
