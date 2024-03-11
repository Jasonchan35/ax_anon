#pragma once

#if AX_USE_Win32Api

#include "axNativeUIWindow_Win32.h"
#include "axNativeUIEventUtil_Win32.h"
#include "../Base/axNativeUIWindow_Base.h"
#include "../axNativeUIScreen.h"

class axNativeUIWindow_Win32 : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_Win32, axNativeUIWindow_Base);
public:
	virtual ~axNativeUIWindow_Win32();

	axNativeUIWindow_Win32(CreateDesc& desc);

	HWND	hwnd() { return _hwnd; }

	LRESULT _handleWin32Event(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIKeyEvent(  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	axNativeUIScreen	screen();

protected:
	virtual void onSetNativeVisible	(bool b) override;
	virtual void onSetNativeActive	(bool b) override;
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
	virtual void doCloseWindow	() override;
	virtual void doDestroyWindow() override;

private:
	axNativeUIEventModifier _getWin32Modifier();

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	static axRect2f s_win32_getWorldRect(HWND hwnd);

	axNativeUIMouseEventButton _mousePressedButtons;
	axNativeUIMouseEventButton _lastMousePressedButtons;

	HWND	_hwnd = nullptr;
};

#endif
