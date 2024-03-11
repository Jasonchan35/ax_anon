#if AX_USE_X11

#include "axNativeUIWindow_X11.h"
#include "axNativeUIEventUtil_X11.h"
#include "../axNativeUIApp.h"

void axNativeUIWindow_X11::EventHandler::onExpose() {
	// if (owner) {
	// 	owner->render(*ctx);
	// }
}

void axNativeUIWindow_X11::EventHandler::onCloseButton() {
	if (!owner) return;
	owner->onCloseButton();
}

void axNativeUIWindow_X11::EventHandler::onCustomDestroyWindow() {
	if (!owner) return;
	ax_delete(owner);
}

void axNativeUIWindow_X11::EventHandler::didDestroy() {
	AX_ASSERT(owner == nullptr);
}

axNativeUIScreen axNativeUIWindow_X11::screen() {
	XWindowAttributes attr;
	XGetWindowAttributes(_dpy, _win, &attr);
	return axNativeUIScreen(attr.screen);
}

void axNativeUIWindow_X11::EventHandler::onActive(bool b) {
	if (!owner) return;
	return owner->onActive(b);
}

void axNativeUIWindow_X11::EventHandler::onSetRect(const axRect2f& rect) {
	if (!owner) return;
	owner->_handling_XEvent_setPos  = true;
	owner->_handling_XEvent_setSize = true;	
	owner->setWorldRect(owner->_getX11WorldRect());
	owner->_handling_XEvent_setPos  = false;
	owner->_handling_XEvent_setSize = false;	
}

void axNativeUIWindow_X11::EventHandler::onMouseEvent(XEvent& ev) {
	if (!owner) return;
	owner->_handleNativeUIMouseEvent(ev);
}

void axNativeUIWindow_X11::EventHandler::onKeyEvent(XEvent&  ev ) {
	if (!owner) return;
	owner->_handleNativeUIKeyEvent(ev);
}

bool axNativeUIWindow_X11::_setX11WinProperty(ax_X11_Atom prop, axSpan<ax_X11_Atom> values, bool printError) {
	int ret = XChangeProperty(_dpy, _win, prop, XA_ATOM, 32,
								ax_X11_PropModeReplace, 
								reinterpret_cast<const unsigned char*>(values.data()), 
								static_cast<int>(values.size()));

	if (ret == 0) return true;

	if (printError) {
		ax_X11_Util::checkError(AX_LOC, ret);

		auto prop_ = axStrView_c_str(XGetAtomName(_dpy, prop));
		axArray<axStrView> values_;
		values_.resize(values.size());
		axInt i = 0;
		for (auto& src : values) {
			values_[i] = axStrView_c_str(XGetAtomName(_dpy, src));
		}
		AX_LOG("Error: XChangeProperty prop={?} values={?}", prop_, values_);
	}
	return false;
}

Atom axNativeUIWindow_X11::_getX11Atom(axStrLiteral name) {
	return XInternAtom(_dpy, name.c_str(), ax_X11_False);
}

axNativeUIWindow_X11::axNativeUIWindow_X11(CreateDesc & desc) 
	: Base(desc)
	, _handling_XEvent_setPos(false)
	, _handling_XEvent_setSize(false)
{
	_eventHandler.owner = this;
	
	auto* app = axNativeUIApp::s_instance();
	auto& atoms = app->_atoms;
	auto xvis = app->xvisualInfo();

	_dpy = app->xdisplay();	
	auto root = app->rootWin();

	unsigned int w = desc.rect.w <= 0 ? 0 : (unsigned int) desc.rect.w;
	unsigned int h = desc.rect.h <= 0 ? 0 : (unsigned int) desc.rect.h;

	unsigned long attr_mask = 0;
	XSetWindowAttributes  attr;

	// attr.background_pixel = XWhitePixel(dpy,screen);
	// attr_mask |= CWBackPixel;

	attr.colormap = app->colormap();
	attr_mask |= ax_X11_CWColormap;

	int depth = 0;
	_win = ::XCreateWindow( _dpy, root,
							(int)desc.rect.x, (int)desc.rect.y, w, h,
							depth,
							xvis->depth, InputOutput, xvis->visual,
							attr_mask, &attr);

	XStoreName(_dpy, _win, "axNativeUIWindow_X11");

	axArray<ax_X11_Atom, 64> actions;

	switch (desc.type) {
		case CreateDesc::Type::NormalWindow: {
			actions.append(atoms._NET_WM_ACTION_MOVE);
			actions.append(atoms._NET_WM_ACTION_SHADE);
			actions.append(atoms._NET_WM_ACTION_STICK);
			actions.append(atoms._NET_WM_ACTION_FULLSCREEN);
			actions.append(atoms._NET_WM_ACTION_CHANGE_DESKTOP);

			if (desc.resizable  ) actions.append(atoms._NET_WM_ACTION_RESIZE);
			if (desc.closeButton) actions.append(atoms._NET_WM_ACTION_CLOSE);
			if (desc.minButton  ) actions.append(atoms._NET_WM_ACTION_MINIMIZE);
			if (desc.maxButton  ) {
				actions.append(atoms._NET_WM_ACTION_MAXIMIZE_HORZ);
				actions.append(atoms._NET_WM_ACTION_MAXIMIZE_VERT);
			}
		}break;
		case CreateDesc::Type::PopupWindow: { 
			_setX11WinProperty(atoms._NET_WM_WINDOW_TYPE, atoms._NET_WM_WINDOW_TYPE_MENU, false);
			
			{
				// use old school "_MOTIF_WM_HINTS" (XFCE doesn't support _NET_WM_WINDOW_TYPE)
				struct MwmHints {
					unsigned long flags;
					unsigned long functions;
					unsigned long decorations;
					long input_mode;
					unsigned long status;
				};
				enum {
					MWM_HINTS_FUNCTIONS 	= (1L << 0),
					MWM_HINTS_DECORATIONS 	= (1L << 1),
				//---
					MWM_FUNC_ALL			= (1L << 0),
					MWM_FUNC_RESIZE 		= (1L << 1),
					MWM_FUNC_MOVE 			= (1L << 2),
					MWM_FUNC_MINIMIZE 		= (1L << 3),
					MWM_FUNC_MAXIMIZE 		= (1L << 4),
					MWM_FUNC_CLOSE 			= (1L << 5),
				//---
					PROP_MWM_HINTS_ELEMENTS = 5,
				};					
				MwmHints hints = {};
				hints.flags = MWM_HINTS_DECORATIONS;
				hints.decorations = 0;
				int ret = XChangeProperty(_dpy, _win, 
											atoms._MOTIF_WM_HINTS, 
											atoms._MOTIF_WM_HINTS,
											32,
											ax_X11_PropModeReplace, 
											reinterpret_cast<const unsigned char*>(&hints), 
											PROP_MWM_HINTS_ELEMENTS);
				ax_X11_Util::checkError(AX_LOC, ret);
			}
		}break;
		case CreateDesc::Type::ToolWindow: {
			_setX11WinProperty(atoms._NET_WM_WINDOW_TYPE, atoms._NET_WM_WINDOW_TYPE_UTILITY);
		}break;
	}

//	_setX11WinProperty(atoms._NET_WM_ALLOWED_ACTIONS, actions);

	setWorldRect(_getX11WorldRect());
	_eventHandler.create(_dpy, _win);

    // X input context, you can have multiple for text boxes etc, but having a
    // single one is the easiest.
	_xic = XCreateIC(app->xim(),
					XNInputStyle,   XIMPreeditNothing | XIMStatusNothing,
					XNClientWindow, _win,
					XNFocusWindow,  _win,
					nullptr);
    XSetICFocus(_xic);

	setVisible(desc.visible);
}

axNativeUIWindow_X11::~axNativeUIWindow_X11() {
	_destroy();
}

void axNativeUIWindow_X11::_destroy() {
	if (_win) {
		auto* app = axNativeUIApp::s_instance();		
		XDeleteContext(_dpy, _win, app->xcontext());
		XDestroyWindow(_dpy, _win);
		_win = 0;
	}
}

Window axNativeUIWindow_X11::_getX11ParentWindow() const {
	Window root = 0;
	Window parent = 0;
	Window* children = nullptr;
	unsigned int childCount = 0;
	XQueryTree(_dpy, _win, &root, &parent, &children, &childCount);
	return parent;
}

axRect2f axNativeUIWindow_X11::_getX11WorldRect() const {
	if (!_dpy || !_win) return axRect2f(0);

	Window root = 0;
	Window child = 0;
	int x, y;
	unsigned int w, h;
	unsigned int border;
	unsigned int depth;
	XGetGeometry(_dpy, _win, &root, &x, &y, &w, &h,&border, &depth);
	XTranslateCoordinates(_dpy, _win, root, 0, 0, &x, &y, &child);

	return axRect2f(static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(w),
					static_cast<float>(h));
}

void axNativeUIWindow_X11::onSetNativeWorldPos(const axVec2f & v) {	
	if (_handling_XEvent_setPos) return;

	auto root = DefaultRootWindow( _dpy );
	auto parent = _getX11ParentWindow();

	int x = static_cast<int>(v.x);
	int y = static_cast<int>(v.y);

	int nx = x;
	int ny = y;

	Window child = 0;
	// get pos relative tp parent
	XTranslateCoordinates(_dpy, root, parent, x, y, &nx, &ny, &child);
	XMoveWindow(_dpy, _win, nx, ny);
}

void axNativeUIWindow_X11::onSetNativeSize(const axVec2f & v) {
	unsigned int w = (unsigned int)ax_max(0.0f, v.x);
	unsigned int h = (unsigned int)ax_max(0.0f, v.y);

	if (!_handling_XEvent_setSize) {
		XResizeWindow(_dpy, _win, w, h);
	}
}

void axNativeUIWindow_X11::onSetNativeWindowTitle(axStrView title) {
	axTempString tmp(title);
	XStoreName(_dpy, _win, tmp.c_str());
}

void axNativeUIWindow_X11::onSetNativeActive(bool b) {
	if (b) {
		XSetInputFocus(_dpy, _win, ax_X11_RevertToNone, ax_X11_CurrentTime);
	} else {
		XSetInputFocus(_dpy, ax_X11_None, ax_X11_RevertToNone, ax_X11_CurrentTime);
	}
}

void axNativeUIWindow_X11::onSetNativeVisible(bool b) {
	if (b) {
		XMapWindow(_dpy, _win);
	} else {
		XUnmapWindow(_dpy, _win);
	}
}

void axNativeUIWindow_X11::doCloseWindow() {
	if (_win) {
		auto* app = axNativeUIApp::s_instance();
		app->sendEvent_WMProtocol(_win, app->_atoms.WM_DELETE_WINDOW);
	}
}

void axNativeUIWindow_X11::doDestroyWindow() {
	if (_win) {		
		auto* app = axNativeUIApp::s_instance();
		app->sendEvent_WMProtocol(_win, app->_atoms.AX_CUSTOM_DESTROY_WINDOW);
	}
}

void axNativeUIWindow_X11::_updateMousePressedButtons	(unsigned int xEventState) {
	_mousePressedButtons = axNativeUIMouseEventButton::None;
	if (xEventState & ax_X11_Button1Mask) _mousePressedButtons |= axNativeUIMouseEventButton::Left;
	if (xEventState & ax_X11_Button2Mask) _mousePressedButtons |= axNativeUIMouseEventButton::Middle;
	if (xEventState & ax_X11_Button3Mask) _mousePressedButtons |= axNativeUIMouseEventButton::Right;
	if (xEventState & ax_X11_Button4Mask) _mousePressedButtons |= axNativeUIMouseEventButton::Button4;
	if (xEventState & ax_X11_Button5Mask) _mousePressedButtons |= axNativeUIMouseEventButton::Button5;
}

void axNativeUIWindow_X11::_updateEventModifier		(unsigned int xEventState) {
	_eventModifier = axNativeUIEventModifier::None;
	if (xEventState & ax_X11_ShiftMask		) _eventModifier |= axNativeUIEventModifier::Shift;
	if (xEventState & ax_X11_ControlMask	) _eventModifier |= axNativeUIEventModifier::Ctrl;
	if (xEventState & ax_X11_Mod1Mask		) _eventModifier |= axNativeUIEventModifier::Atl;
	if (xEventState & ax_X11_Mod4Mask		) _eventModifier |= axNativeUIEventModifier::Cmd;
//	AX_DUMP_VAR(xEventState, _eventModifier);
}

void axNativeUIWindow_X11::_handleNativeUIMouseEvent(XEvent& inEvent) {
	axNativeUIMouseEvent ev;
	
	switch (inEvent.type) {
		case ax_X11_MotionNotify: {
			ev.type = axNativeUIMouseEventType::Move;

			auto& xe = inEvent.xmotion;
			_updateEventModifier(xe.state);
			_updateMousePressedButtons(xe.state);			
			ev.worldPos.x = static_cast<float>(xe.x_root);
			ev.worldPos.y = static_cast<float>(xe.y_root);
			ev.modifier = _eventModifier;
		}break;

		case ax_X11_ButtonPress:
		case ax_X11_ButtonRelease: {
			switch (inEvent.type) {
				case ax_X11_ButtonPress:	ev.type = axNativeUIMouseEventType::Down;	break;
				case ax_X11_ButtonRelease: ev.type = axNativeUIMouseEventType::Up;		break;
			}

			auto& xe = inEvent.xbutton;
			
			if (xe.button == ax_X11_Button4 || xe.button == ax_X11_Button5) {
				if (inEvent.type != ax_X11_ButtonPress) return;
				ev.type = axNativeUIMouseEventType::Wheel;

				const float d = 120.0f;
				ev.wheelDelta.y =(xe.button == ax_X11_Button4) ? d : -d;

			} else switch (xe.button) {
				case ax_X11_Button1: ev.button = axNativeUIMouseEventButton::Left;		break;
				case ax_X11_Button2: ev.button = axNativeUIMouseEventButton::Middle;	break;
				case ax_X11_Button3: ev.button = axNativeUIMouseEventButton::Right;		break;
				case ax_X11_Button8: ev.button = axNativeUIMouseEventButton::Button4;	break;
				case ax_X11_Button9: ev.button = axNativeUIMouseEventButton::Button5;	break;
			}

			_updateEventModifier(xe.state);
			_updateMousePressedButtons(xe.state);
			ev.worldPos.x = static_cast<float>(xe.x_root);
			ev.worldPos.y = static_cast<float>(xe.y_root);
			ev.modifier = _eventModifier;
		}break;
		
		default: AX_ASSERT(false); return;
	}

	_onNativeUIMouseEvent(ev);
	_lastMousePressedButtons = _mousePressedButtons;
}

void axNativeUIWindow_X11::_handleNativeUIKeyEvent(XEvent& inEvent) {
	axNativeUIKeyEvent ev;
	using Type = axNativeUIKeyEventType;

	switch (inEvent.type) {
		case ax_X11_KeyPress: 	ev.type = Type::Down; break;
		case ax_X11_KeyRelease:	ev.type = Type::Up;   break;
		default: AX_ASSERT(false); return;
	}	

	//auto* app = axNativeUIApp_X11::s_instance();
	//auto* xim = app->x11_xim();

	auto& xe = inEvent.xkey;

//	AX_DUMP_VAR(xe.state, xe.keycode);

	_updateEventModifier(xe.state);
	ev.modifier = _eventModifier;

	KeySym keysym = ax_X11_NoSymbol;

	if (ev.type == Type::Up) {
		keysym =  XLookupKeysym(&xe, 0);

	} else if (ev.type == Type::Down) {
		ax_X11_Status status;
		char text[32];
		text[0] = 0;

		// if you want to tell if this was a repeated key, this trick seems reliable.
		// int is_repeat = prev_ev.type         == KeyRelease &&
		// 				prev_ev.xkey.time    == ev.xkey.time &&
		// 				prev_ev.xkey.keycode == ev.xkey.keycode;

		// you might want to remove the control modifier, since it makes stuff return control codes
		// ev.xkey.state &= ~ControlMask;

		// get text from the key.
		// it could be multiple characters in the case an IME is used.
		// if you only care about latin-1 input, you can use XLookupString instead
		// and skip all the XIM / XIC setup stuff

		int ret = Xutf8LookupString(_xic, &xe, text, sizeof(text) - 1, &keysym, &status);
		if (ret > 0) {
			text[ret] = 0;

			switch (status) {
				case XBufferOverflow: {
					// an IME was probably used, and wants to commit more than 32 chars.
					// ignore this fairly unlikely case for now
					return;
				}break;

				case XLookupChars: {
					// some characters were returned without an associated key,
					// again probably the result of an IME
					//printf("Got chars: (%s)\n", text);
					// AX_LOG("XLookupChars = {?}", axStrView_c_str(text));

					ev.text.setUtf(axStrView_c_str(text));
				}break;

				case XLookupBoth: {
					// we got one or more characters with an associated keysym
					// (all the keysyms are listed in /usr/include/X11/keysymdef.h)
					//char* sym_name = XKeysymToString(keysym);
					//printf("Got both: (%s), (%s)\n", text, sym_name);
					//AX_LOG("XLookupBoth = {?} {?}", axStrView_c_str(text), axStrView_c_str(sym_name));

					ev.text.setUtf(axStrView_c_str(text));
				}break;

				case XLookupKeySym: {
					// a key without text on it
					//char* sym_name = XKeysymToString(keysym);
					//printf("Got keysym: (%s)\n", sym_name);
					//AX_LOG("XLookupKeySym = {?} {?}", axStrView_c_str(sym_name), keysym);
				}break;
			}

			if (ev.text.size()) {
				ev.type = Type::Char;
				_onNativeUIKeyEvent(ev);
				ev.text.clear();
				ev.type = Type::Down;
			}
		}
	}

//	char* sym_name = XKeysymToString(keysym);
//	AX_LOG("key {?:04X} {?}", keysym, sym_name);

	ev.key = ax_X11_Util::convertKey(keysym);

//	AX_DUMP_VAR(ev);
	_onNativeUIKeyEvent(ev);
}

#endif //AX_USE_X11
