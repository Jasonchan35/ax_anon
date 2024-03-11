#if AX_USE_Win32Api

#include <ax_core/file/axPath.h>
#include "../axNativeUIApp.h"
#include "axNativeUIApp_Win32.h"
#include "axNativeUIWindow_Win32.h"

int axNativeUIApp_Win32::onRun() {
	_createHiddenWindow();

	_cursorUnknown			= LoadCursor(nullptr, IDC_ARROW);
//-----
	_cursorArrow			= LoadCursor(nullptr, IDC_ARROW);
	_cursorClosedHand		= LoadCursor(nullptr, IDC_CROSS);
	_cursorContextualMenu	= nullptr;
	_cursorCrossHair		= nullptr;
	_cursorDisappearingItem = nullptr;
	_cursorDragCopy			= nullptr;
	_cursorDragLink			= nullptr;
	_cursorHelp				= LoadCursor(nullptr, IDC_HELP);
	_cursorIBeam			= LoadCursor(nullptr, IDC_IBEAM);
	_cursorIBeamVertical	= nullptr;
	_cursorNotAllowed		= LoadCursor(nullptr, IDC_NO);
	_cursorOpenHand			= LoadCursor(nullptr, IDC_SIZEALL); //! need custom icon
	_cursorPointingHand		= LoadCursor(nullptr, IDC_HAND);
	_cursorResizeAll		= LoadCursor(nullptr, IDC_SIZEALL);
	_cursorResizeNESW		= LoadCursor(nullptr, IDC_SIZENESW);
	_cursorResizeNS			= LoadCursor(nullptr, IDC_SIZENS);
	_cursorResizeNWSE		= LoadCursor(nullptr, IDC_SIZENWSE);
	_cursorResizeWE			= LoadCursor(nullptr, IDC_SIZEWE);
	_cursorShortWait		= LoadCursor(nullptr, IDC_APPSTARTING);
	_cursorWait				= LoadCursor(nullptr, IDC_WAIT);

	// IDC_UPARROW

	create();

	while (GetMessage(&_win32_msg, NULL, 0, 0)) {
		TranslateMessage(&_win32_msg);
		DispatchMessage(&_win32_msg);
		axNativeUIApp::s_instance()->handleCustomAppEvent(false);
	}

	willQuit();
	return _returnCode;
}

void axNativeUIApp_Win32::quit(int returnCode) {
	_returnCode = returnCode;
	::PostQuitMessage(returnCode);
}

axRect2f axNativeUIApp_Win32::s_getVirtualDesktopRect() {
	return axRect2f(0, 0,
					(float)GetSystemMetrics(SM_CXVIRTUALSCREEN), 
					(float)GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

bool axNativeUIApp_Win32::onSetNativeCursor(axUICursorType type) {
	HCURSOR o = nullptr;
	switch (type) {
		#define AX_MACRO_OP(E, VALUE) case axUICursorType::E: o = _cursor##E; break;
			axUICursorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
	}

	if (!o) {
		AX_ASSERT(false);
		o = _cursorArrow;
	}
	::SetCursor(o);

	return true;
}

bool axNativeUIApp_Win32::onSetNativeCursorPos(const axVec2f& worldPos) {
	auto ret = ::SetCursorPos(static_cast<int>(worldPos.x), static_cast<int>(worldPos.y));
	return ret != 0;
}

bool axNativeUIApp_Win32::onRequestNativeCustomAppEvent() {
	if (!_hiddenWindow)
		return false;

	if (!::PostMessage(_hiddenWindow, ax_WM_USER_HasCustomAppEvent, 0, 0))
		return false;
	return true;
}

LRESULT WINAPI axNativeUIApp_Win32::s_hiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case ax_WM_USER_HasCustomAppEvent: {
			axNativeUIApp::s_instance()->handleCustomAppEvent(false);
		}break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void axNativeUIApp_Win32::_createHiddenWindow() {
	const wchar_t* clsName = L"axNativeUIApp_HiddenWindow";

	auto hInstance = ::GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= &s_hiddenWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= nullptr;
	wc.hCursor			= nullptr;
	wc.hbrBackground	= nullptr;
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	DWORD dwStyle = 0;
	DWORD dwExStyle = 0;

	if (!::RegisterClassEx(&wc)) {
		throw axError_Undefined(AX_LOC);
	}

	_hiddenWindow = ::CreateWindowExW(dwExStyle, clsName, clsName, dwStyle, 
								0, 0, 0, 0, 
								nullptr, nullptr, hInstance, nullptr);
	if (!_hiddenWindow) {
		throw axError_Undefined(AX_LOC);
	}	
}

#endif //AX_OS_WINDOWS
