#if AX_USE_X11

#include "../axNativeUIApp.h"
#include "../axNativeUITimer.h"
#include "axNativeUIEventUtil_X11.h"

axNativeUIApp_X11::axNativeUIApp_X11() {
	XInitThreads(); // X11 - support Multi-thread

	_xcontext = XUniqueContext();
	if (!_xcontext) throw axError_Undefined(AX_LOC);

	_dpy = XOpenDisplay(nullptr);
	if (!_dpy) throw axError_Undefined(AX_LOC);

	_cursorUnknown			= XCreateFontCursor(_dpy, XC_arrow);
//-----
	_cursorArrow			= XCreateFontCursor(_dpy, XC_arrow);
//	_cursorClosedHand		= 0;
//	_cursorContextualMenu	= 0;
	_cursorCrossHair		= XCreateFontCursor(_dpy, XC_crosshair);
//	_cursorDisappearingItem = 0;
	_cursorDragCopy			= XCreateFontCursor(_dpy, XC_draped_box);
//	_cursorDragLink			= XCreateFontCursor(_dpy, XC_);
	_cursorHelp				= XCreateFontCursor(_dpy, XC_question_arrow);
	_cursorIBeam			= XCreateFontCursor(_dpy, XC_xterm);
//	_cursorIBeamVertical	= 0;
	_cursorNotAllowed		= XCreateFontCursor(_dpy, XC_X_cursor);
//	_cursorOpenHand			= 0;
	_cursorPointingHand		= XCreateFontCursor(_dpy, XC_hand1);
	_cursorResizeAll		= XCreateFontCursor(_dpy, XC_fleur);
	_cursorResizeNESW		= XCreateFontCursor(_dpy, XC_bottom_left_corner);
	_cursorResizeNS			= XCreateFontCursor(_dpy, XC_sb_v_double_arrow);
	_cursorResizeNWSE		= XCreateFontCursor(_dpy, XC_bottom_right_corner);
	_cursorResizeWE			= XCreateFontCursor(_dpy, XC_sb_h_double_arrow);
	_cursorShortWait		= XCreateFontCursor(_dpy, XC_watch);
	_cursorWait				= XCreateFontCursor(_dpy, XC_watch);


	_rootWin = DefaultRootWindow(_dpy);

	_fd = ConnectionNumber(_dpy);

	XSetErrorHandler(&_x11ErrorHandler);
	_atoms.init(_dpy);
}

axNativeUIApp_X11::~axNativeUIApp_X11() {
	if (_dpy) {
		XCloseDisplay(_dpy);
	}
}

int axNativeUIApp_X11::_x11ErrorHandler(Display* dpy, XErrorEvent* err) {
#if _DEBUG
	axStrView errStr("Unknown");
	switch (err->error_code) {
		#define ax_entry(v) case ax_X11_##v: errStr = #v; break;
			ax_entry(Success);
			ax_entry(BadRequest);
			ax_entry(BadValue);
			ax_entry(BadWindow);
			ax_entry(BadPixmap);
			ax_entry(BadAtom);
			ax_entry(BadCursor);
			ax_entry(BadFont);
			ax_entry(BadMatch);
			ax_entry(BadDrawable);
			ax_entry(BadAccess);
			ax_entry(BadAlloc);
			ax_entry(BadColor);
			ax_entry(BadGC);
			ax_entry(BadIDChoice);
			ax_entry(BadName);
			ax_entry(BadLength);
			ax_entry(BadImplementation);
		#undef ax_entry
	}
	AX_LOG("X11ErrorHandler {?}:[{?}]", err->error_code, errStr);
#endif

//	throw axError_Undefined(AX_LOC);
	return 0;
}

void axNativeUIApp_X11::AtomList::init(ax_X11_Display* dpy) {
	#define E(T) T = XInternAtom(dpy, #T, ax_X11_False);
		axNativeUIApp_X11_AtomList(E)
	#undef aE
}

int axNativeUIApp_X11::onRun() {
	_os_init();

	create();
	_os_run();

	willQuit();

	_os_release();

	return _returnCode;
}

void axNativeUIApp_X11::_os_init() {
	GLint attrib[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 24,
		0 // - End -
	};
	_xvisualInfo = glXChooseVisual(_dpy, 0, attrib);
	if (_xvisualInfo == nullptr) throw axError_Undefined(AX_LOC);

	_colormap  = XCreateColormap(_dpy, _rootWin, _xvisualInfo->visual, ax_X11_AllocNone); 

// ---- input -----
    XSetLocaleModifiers("");

    _xim = XOpenIM(_dpy, 0, 0, 0);
    if(!_xim){
        // fallback to internal input method
        XSetLocaleModifiers("@im=none");
        _xim = XOpenIM(_dpy, 0, 0, 0);
    }
}

void axNativeUIApp_X11::_os_release() {
	XFreeColormap(_dpy, _colormap);
}

bool axNativeUIApp_X11::_timedWaitXEvent(struct timeval* tv) {
	if (XPending(_dpy)) {
		return true;
	}

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(_fd, &readset);

	if (0 == select(_fd + 1, &readset, nullptr, nullptr, tv)) {
		return false;
	}
	return true;
}

axNativeUIEventTime axNativeUIApp_X11::_nextTimerTimestamp() {
	Time o;
	o.setSeconds(axNumLimit<Time::Int>::max());
	for (auto& t : _timers) {
		ax_min_it(o, t->_nextFireTime);
	}
	return o;
}

void axNativeUIApp_X11::_fireTimers() {
	auto now = Time::s_now();

	for (auto& t : _timers) {
		if (t->_nextFireTime <= now) {
			t->_fire();
			t->_updateNextFireTime();
		}
	}
}

void axNativeUIApp_X11::_os_run() {
	XEvent  ev;

	handleCustomAppEvent(true);

//------------
	for(;;) {
		if (_shouldQuit) {
			break;
		}

		auto waitTime = _nextTimerTimestamp() - Time::s_now();
		ax_min_it(waitTime, Time::s_milliseconds(10L)); // no more than 10ms

		timeval tv;
		tv.tv_sec  = waitTime.toSeconds();
		tv.tv_usec = waitTime.toMicroseconds();

		_timedWaitXEvent(&tv);
		_fireTimers();
//		XPeekEvent(_dpy, &ev); // wait for event

		//!! work around, don't know why CustomAppEvent doesn't work all the time !!
		handleCustomAppEvent(true); 

		int evCount = XPending(_dpy);
//		AX_LOG("XEvent in batch {?}", evCount);

		for (int i = 0; i < evCount; i++) {
			XNextEvent(_dpy, &ev);
			_handleXEvent(ev);
		}

		for (auto& h : _exposeList) {
			if (!h) continue;
			h->onExpose();
			h->_inExposeList = false;
		}
		_exposeList.clear();
	}
}

void axNativeUIApp_X11::_addHandlerNeedExpose(ax_X11_Window win, ax_X11_EventHandler& h) {
	if (h._inExposeList)
		return;

	h._inExposeList = true;
	_exposeList.emplaceBack(&h);
	XClearArea(_dpy, win, 0, 0, 0, 0, true); // send Expose event
}

void axNativeUIApp_X11::_handleXEvent(XEvent& ev) {
//	AX_LOG("  handle XEvent {?}", ev.type);

	switch (ev.type) {
		case ax_X11_PropertyNotify: {
			auto& xe = ev.xproperty;
			if (xe.atom == _atoms._NET_ACTIVE_WINDOW) {
//				AX_DUMP_VAR(xe.window, xe.atom);
			}
		}break;
	}

//----- Custom Event without Window or ax_X11_EventHandler
	switch (ev.type) {
		case ax_X11_ClientMessage: {
			auto& c = ev.xclient;
			auto msg = c.message_type;
			if (msg == _atoms.AX_CUSTOM_APP_EVENT) {
				axNativeUIApp_Base::s_instance()->handleCustomAppEvent(true);
			}
		}
	}

//---------------------------
	XPointer handlerVoid = nullptr;
	if (0 != XFindContext(_dpy, ev.xany.window, _xcontext, &handlerVoid)) {
		return;
	}
	if (!handlerVoid) return;

	auto* handler = reinterpret_cast<ax_X11_EventHandler*>(handlerVoid);
	if (!handler->checkSignature()) {
		throw axError_Undefined(AX_LOC);
	}

	switch (ev.type) {
		case ax_X11_Expose: {
			if (!handler->_inExposeList) {
				handler->_inExposeList = true;
				_exposeList.emplaceBack(handler);
			}
		}break;

		case ax_X11_DestroyNotify: {
			handler->didDestroy();
		}break;
		
		case ax_X11_ConfigureNotify: {
			auto& c = ev.xconfigure;
			axRect2f rect(	static_cast<float>(c.x),
							static_cast<float>(c.y),
							static_cast<float>(c.width),
							static_cast<float>(c.height));
			handler->onSetRect(rect);
		}break;

		case ax_X11_FocusIn: 	handler->onActive(true);  break;
		case ax_X11_FocusOut: 	handler->onActive(false); break;

		case ax_X11_KeyPress:
		case ax_X11_KeyRelease: {
			handler->onKeyEvent(ev);
		}break;

		case ax_X11_ButtonPress:
		case ax_X11_ButtonRelease:
		case ax_X11_MotionNotify: {
			handler->onMouseEvent(ev);
		}break;

		case ax_X11_ClientMessage: {
			auto& c = ev.xclient;
			auto msg = c.message_type;
			if (msg == _atoms.WM_PROTOCOLS) {
				auto subType = (uint32_t)c.data.l[0]; 
				if (subType == _atoms.WM_DELETE_WINDOW) {					
					handler->onCloseButton(); //when user click close button
				}
			} else if (msg == _atoms.AX_CUSTOM_DESTROY_WINDOW) {
				handler->onCustomDestroyWindow();
			}
		}break;
	}
}

ax_X11_Status axNativeUIApp_X11::sendEvent_ClientMessage(ax_X11_Window win, ax_X11_Atom msg, long p0, long p1, long p2, long p3, long p4) {
	XEvent ev = {};
	auto& c = ev.xclient;
	c.type = ax_X11_ClientMessage;
	c.window = win;
	c.message_type = msg;
	c.send_event = ax_X11_True;
	c.format = 32; // 32 bits
	c.data.l[0] = p0;
	c.data.l[1] = p1;
	c.data.l[2] = p2;
	c.data.l[3] = p3;
	c.data.l[4] = p4;

	auto ret = XSendEvent(_dpy, win, false, ax_X11_NoEventMask, &ev);
	// if (ret != 0) {
	// 	auto msgStr = axStrView_c_str(XGetAtomName(_dpy, msg));
	// 	AX_LOG("error sendEvent_ClientMessage({?}) ret={?}", msgStr, ret);
	// }
	return ret;
}

ax_X11_Status axNativeUIApp_X11::sendEvent_WMProtocol(ax_X11_Window win, ax_X11_Atom subType, long p1, long p2, long p3, long p4) {
	auto ret = sendEvent_ClientMessage(win, _atoms.WM_PROTOCOLS, subType, p1, p2, p3, p4);
	if (ret != 0) {
		auto subTypeStr = axStrView_c_str(XGetAtomName(_dpy, subType));
		AX_LOG("error sendEvent_WMProtocol({?}) ret={?}", subTypeStr, ret);
	}
	return ret;
}

bool axNativeUIApp_X11::onRequestNativeCustomAppEvent() {
	if (0 != sendEvent_ClientMessage(0, _atoms.AX_CUSTOM_APP_EVENT))
		return false;		
	return true;
}

void axNativeUIApp_X11::quit(int returnCode) {
	_returnCode = returnCode;
	_shouldQuit = true;

	XEvent ev;
	auto& e = ev.xexpose;
	e.display = _dpy;
	e.type = ax_X11_Expose;
	e.window = 0;

	//put back a dummy event, so it can unblock XNextEvent in _onRun() loop
	XPutBackEvent(_dpy, &ev);
}

axRect2f axNativeUIApp_X11::s_getVirtualDesktopRect() {	
	if (auto* display = XOpenDisplay(nullptr)) {
		if (auto* screen = DefaultScreenOfDisplay(display)) {
			int w = WidthOfScreen(screen);
			int h = HeightOfScreen(screen);
			return axRect2f(0, 0, w, h); 
		}
	}
	return axRect2f(0,0, 0, 0);
}

bool axNativeUIApp_X11::onSetNativeCursor(axUICursorType type) {
	Cursor o = 0;
	switch (type) {
		#define AX_MACRO_OP(E, VALUE) case axUICursorType::E: o = _cursor##E; break;
			axUICursorType_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
	}

	if (o == 0) return false;
	XDefineCursor(_dpy, _rootWin, o);
	return true;
}

bool axNativeUIApp_X11::onSetNativeCursorPos(const axVec2f& worldPos) {
	auto pos = axVec2<int>::s_cast(worldPos);
	XWarpPointer(_dpy, 0, _rootWin, 
					0,0,0,0,
					pos.x, pos.y);
	return true;
}

void ax_X11_EventHandler::create(ax_X11_Display* dpy, ax_X11_Window win) {
	auto* app = axNativeUIApp::s_instance();
	auto& atoms = app->_atoms;

	XSaveContext(dpy, win, app->xcontext(), (XPointer)this);
	XSelectInput(dpy, win, ax_X11_StructureNotifyMask 
						| ax_X11_ExposureMask 
					//	| ax_X11_FocusChangeMask 
						| ax_X11_KeyPressMask
						| ax_X11_KeyReleaseMask
						| ax_X11_ButtonPressMask
						| ax_X11_ButtonReleaseMask
						| ax_X11_EnterWindowMask
						| ax_X11_LeaveWindowMask
						| ax_X11_PointerMotionMask
						| ax_X11_Button1MotionMask
						| ax_X11_Button2MotionMask
						| ax_X11_Button3MotionMask
						| ax_X11_Button4MotionMask
						| ax_X11_Button5MotionMask
						| ax_X11_ButtonMotionMask);
		
	Atom protocol[] = {
		atoms.WM_DELETE_WINDOW
	};

	XSetWMProtocols(dpy, win, protocol, ax_c_array_size(protocol));
}

#endif // AX_USE_X11
