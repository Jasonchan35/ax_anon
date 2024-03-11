#if AX_USE_Win32Api
#include "axNativeUIWindow_Win32.h"
#include "axNativeUIEventUtil_Win32.h"
#include <ax_core/native_ui/axNativeUIApp.h>
#include <ax_core/rtti/axTypeManager.h>

axNativeUIWindow_Win32::~axNativeUIWindow_Win32() {
	if (_hwnd) {
		::SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
		::DestroyWindow(_hwnd);
	}
}

LRESULT WINAPI axNativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
				thisObj->_hwnd = nullptr;
				ax_delete(thisObj);
			}
		}break;

		case WM_CLOSE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onCloseButton();
				return 0;
			}
		}break;

		case WM_ACTIVATE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				axUInt16 a = LOWORD(wParam);
				switch (a) {
					case WA_ACTIVE:		thisObj->onActive(true);  break;
					case WA_CLICKACTIVE:thisObj->onActive(true);  break;
					case WA_INACTIVE:	thisObj->onActive(false); break;
				}
			}
		}break;

		case WM_ENTERSIZEMOVE: {
			// TODO
		}break;

		case WM_EXITSIZEMOVE: {
			// TODO
		}

		case WM_MOVE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onSetWorldPos(s_win32_getWorldRect(hwnd).pos);
			}
		}break;

		case WM_SIZING: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->setWorldRect(s_win32_getWorldRect(hwnd));
			}
		}break;

		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->setWorldRect(s_win32_getWorldRect(hwnd));
			}
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_handleWin32Event(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

axNativeUIScreen axNativeUIWindow_Win32::screen() {
	if (_hwnd) {
		return MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
	}
	return axNativeUIScreen();
}

axRect2f axNativeUIWindow_Win32::s_win32_getWorldRect(HWND hwnd) {
	RECT rc;
	::GetWindowRect(hwnd, &rc);

	axRect2f o(rc);

	if (auto parentHwnd = ::GetParent(hwnd)) {
		POINT pt = {rc.left, rc.top};
		MapWindowPoints(hwnd, nullptr, &pt, 1);
		o.pos = axVec2f(pt);
	}
	return o;
}

axNativeUIWindow_Win32::axNativeUIWindow_Win32(CreateDesc& desc) 
	: Base(desc) 
{
	const wchar_t* clsName = L"axNativeUIWindow";

	auto hInstance = ::GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW; // | CS_DROPSHADOW;
	wc.lpfnWndProc		= &s_wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	if (!desc.closeButton) {
		wc.style |= CS_NOCLOSE;
	}

	DWORD dwStyle = 0;
	DWORD dwExStyle = WS_EX_ACCEPTFILES;

	switch (desc.type) {
		case CreateDesc::Type::ToolWindow:
		case CreateDesc::Type::NormalWindow: {
			dwStyle   |= WS_OVERLAPPED | WS_SYSMENU;

			if (desc.closeButton) dwStyle |= WS_SYSMENU;
			if (desc.resizable  ) dwStyle |= WS_THICKFRAME;
			if (desc.titleBar   ) dwStyle |= WS_CAPTION;
			if (desc.minButton  ) dwStyle |= WS_MINIMIZEBOX;
			if (desc.maxButton  ) dwStyle |= WS_MAXIMIZEBOX;
		}break;

		case CreateDesc::Type::PopupWindow: {
			dwStyle   |= WS_POPUP | WS_BORDER;
		}break;
		default: AX_ASSERT(false); break;
	}

	if (desc.type == CreateDesc::Type::ToolWindow) {
		dwExStyle |= WS_EX_TOOLWINDOW;
	}

	if (desc.alwaysOnTop) dwExStyle |= WS_EX_TOPMOST;

	WNDCLASSEX tmpWc;
	bool registered = (0 != ::GetClassInfoEx(hInstance, clsName, &tmpWc));
	if (!registered) {
		if (!::RegisterClassEx(&wc)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	auto rect = desc.rect;
	if (desc.centerToScreen) {
		auto screenSize = axVec2f((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));
		rect.pos = (screenSize - rect.size) / 2;
	}

	_hwnd = ::CreateWindowExW(dwExStyle, clsName, clsName, dwStyle,
								(int)desc.rect.x,
								(int)desc.rect.y,
								(int)desc.rect.w,
								(int)desc.rect.h,
								nullptr, nullptr, hInstance, this);
	if (!_hwnd) {
		throw axError_Undefined(AX_LOC);
	}

	setWorldRect(s_win32_getWorldRect(_hwnd));

	setVisible(desc.visible);
}

void axNativeUIWindow_Win32::onSetNativeWorldPos(const axVec2f& v) {
	if (!_hwnd) { AX_ASSERT(false); return; }

	POINT pt {static_cast<LONG>(v.x), static_cast<LONG>(v.y)};

	if (auto parentHwnd = ::GetParent(_hwnd)) {
		// get pos relative to parent
		::MapWindowPoints(nullptr, parentHwnd, &pt, 1);
	}

	RECT rc;
	::GetWindowRect(_hwnd, &rc);
	auto w = rc.right  - rc.left;
	auto h = rc.bottom - rc.top;

	::MoveWindow(_hwnd, pt.x, pt.y, w, h, false);
}

void axNativeUIWindow_Win32::onSetNativeSize(const axVec2f& v) {
	if (!_hwnd) { AX_ASSERT(false); return; }

	RECT rc_{0,0,0,0};
	RECT clientRc_{0,0,0,0};
	::GetWindowRect(_hwnd, &rc_);
	::MoveWindow(_hwnd, rc_.left, rc_.top, (int)v.x, (int)v.y, true);

	// get again after MoveWindow
	::GetWindowRect(_hwnd, &rc_);
	::GetClientRect(_hwnd, &clientRc_);
	
	axRect2f rc(rc_);
	axRect2f clientRc(clientRc_);

	POINT pt_{0,0};
	::ClientToScreen(_hwnd, &pt_);
	axVec2f pt(pt_);

	clientRc.pos += pt;

	Base::setPadding(rc - clientRc);
}

void axNativeUIWindow_Win32::onSetNativeWindowTitle(axStrView title) {
	axTempStringW t;
	t.setUtf(title);

	if (_hwnd) {
		SetWindowText(_hwnd, t.c_str());
	}
}

void axNativeUIWindow_Win32::doCloseWindow() {
	::SendMessage(_hwnd, WM_CLOSE, 0, 0);
}

void axNativeUIWindow_Win32::doDestroyWindow() {
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

void axNativeUIWindow_Win32::onSetNativeVisible(bool b) {
	::ShowWindow(_hwnd, b ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void axNativeUIWindow_Win32::onSetNativeActive(bool b) {
	if (b) {
		::SetActiveWindow(_hwnd);
	} else {
		if (_hwnd == ::GetActiveWindow()) {
			::SetActiveWindow(nullptr);
		}
	}
}

LRESULT axNativeUIWindow_Win32::_handleWin32Event(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (_handleNativeUIKeyEvent  (hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIMouseEvent(hwnd, msg, wParam, lParam)) return 0;
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

axNativeUIEventModifier axNativeUIWindow_Win32::_getWin32Modifier() {
	auto o = axEnum<axNativeUIEventModifier>::kZero;
	if (::GetAsyncKeyState(VK_CONTROL)) o |= axNativeUIEventModifier::Ctrl;
	if (::GetAsyncKeyState(VK_SHIFT  )) o |= axNativeUIEventModifier::Shift;
	if (::GetAsyncKeyState(VK_MENU   )) o |= axNativeUIEventModifier::Atl;
	if (::GetAsyncKeyState(VK_LWIN) || ::GetAsyncKeyState(VK_RWIN)) {
		o |= axNativeUIEventModifier::Cmd;
	}
	return o;
}

bool axNativeUIWindow_Win32::_handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto* app = axNativeUIApp::s_instance();
	if (!app) return false;

	using Type = axNativeUIMouseEvent::Type;
	using Button = axNativeUIMouseEvent::Button;
	using Modifier = axNativeUIMouseEvent::Modifier;

	axNativeUIMouseEvent ev;
	ev.time.setMilliseconds(app->_win32_msg.time);
	ev.modifier = _getWin32Modifier();

#if 0
	// !! From Windows Document:
	// Important  Do not use the LOWORD or HIWORD macros to extract the x- and y- coordinates of the cursor position 
	// because these macros return incorrect results on systems with multiple monitors. 
	// Systems with multiple monitors can have negative x- and y- coordinates, and LOWORD and HIWORD treat the coordinates as unsigned quantities.

	//ev.worldPos = axWin32_LPARAM_to_Vec2f(lParam) + posRelativeToWindow;
#else
	POINT curPos;
	::GetCursorPos(&curPos);

	axVec2f pos(curPos);

	ev.worldPos = pos;
#endif

	auto xbutton = Button::None;
	uint16_t wParamHi = HIWORD(wParam);

	switch(wParamHi) {
		case XBUTTON1: xbutton = Button::Button4; break;
		case XBUTTON2: xbutton = Button::Button5; break;
	}

	switch (msg) {
		//case WM_LBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Left;   } break;
		//case WM_MBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Middle; } break;
		//case WM_RBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Right;  } break;
		//case WM_XBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = xbutton;		} break;

		case WM_LBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Left;   } break;
		case WM_MBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Middle; } break;
		case WM_RBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Right;  } break;
		case WM_XBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = xbutton;		} break;

		case WM_LBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Left;   } break;
		case WM_MBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Middle; } break;
		case WM_RBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Right;  } break;
		case WM_XBUTTONUP:		{ ev.type = Type::Up;		ev.button = xbutton;		} break;

		case WM_MOUSEMOVE:		{ ev.type = Type::Move;		ev.button = Button::None;   } break;

		#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
			case WM_MOUSEWHEEL:		{ ev.type = Type::Wheel;	ev.wheelDelta.set(0,GET_WHEEL_DELTA_WPARAM(wParam)); } break;
		#endif

		#if (_WIN32_WINNT >= 0x0600)
			case WM_MOUSEHWHEEL:	{ ev.type = Type::Wheel;	ev.wheelDelta.set(GET_WHEEL_DELTA_WPARAM(wParam),0); } break;
		#endif

		default:
			return false;
	}	

	ax_enum(_mousePressedButtons).setAllZero();
	if (wParam & MK_LBUTTON)  ax_enum(_mousePressedButtons).setFlags(Button::Left);
	if (wParam & MK_MBUTTON)  ax_enum(_mousePressedButtons).setFlags(Button::Middle);
	if (wParam & MK_RBUTTON)  ax_enum(_mousePressedButtons).setFlags(Button::Right);
	if (wParam & MK_XBUTTON1) ax_enum(_mousePressedButtons).setFlags(Button::Button4);
	if (wParam & MK_XBUTTON2) ax_enum(_mousePressedButtons).setFlags(Button::Button5);

//	ev.pressedButtons = _mousePressedButtons;

	if (ev.type == Type::Down) {
		if (ax_enum(_lastMousePressedButtons).isZero()) {
			::SetCapture(hwnd);
		}

	}else if (ev.type == Type::Up) {
		if (ax_enum(_mousePressedButtons).isZero()) {
			::ReleaseCapture();
		}
	}

//	AX_DUMP_VAR(ev);
	_onNativeUIMouseEvent(ev);
	_lastMousePressedButtons = _mousePressedButtons;
	return true;
}

bool axNativeUIWindow_Win32::_handleNativeUIKeyEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto* app = axNativeUIApp::s_instance();
	if (!app) return false;

	axNativeUIKeyEvent ev;
	using Type = axNativeUIKeyEventType;

	ev.time.setMilliseconds(app->_win32_msg.time);
	ev.modifier	= _getWin32Modifier();
	ev.key = axNativeUIEventUtil_Win32::convertKey(wParam);

	if (msg == WM_CHAR) {
		ev.type = Type::Char;
		axChar32 ch = static_cast<axChar32>(wParam);
		ev.text.setUtf(ch);

	} else {
		switch (msg) {
			case WM_KEYUP:		ev.type = Type::Up;	break;
			case WM_KEYDOWN:	ev.type = Type::Down; break;
			case WM_SYSKEYUP:	ev.type = Type::Up;	break;
			case WM_SYSKEYDOWN: ev.type = Type::Down; break;
			default: return false;
		}

		if (ev.type == axNativeUIKeyEventType::Down) {
			ev.isRepeat = (lParam & (1<<30)) != 0;
			// if (ev.isRepeat) return; // don't send repeat down
		}
	}

//	AX_DUMP_VAR(ev);
	_onNativeUIKeyEvent(ev);
	return true;
}

/*
axVec2f axNativeUIWindow_toMonitorSpace(HWND hwnd, const Vec2f v) {
	auto m = ::MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
	if (!m) return v;
	MONITORINFOEX p;
	info.cbSize = sizeof(info);
	::GetMonitorInfo(m, &info);
	auto rc = info.rcWork;
}
*/

//axRect2f	axNativeUIWindow::windowRect		() const { RECT rc; ::GetWindowRect(_hwnd, &rc); return axRect2f(rc); }
//axRect2f	axNativeUIWindow::contentRect		() const { RECT rc; ::GetClientRect(_hwnd, &rc); return axRect2f(rc); }
//void		axNativeUIWindow::setWindowRect	(const axRect2f& rect) { ::MoveWindow(_hwnd, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, true); }

#endif //AX_OS_WINDOWS
