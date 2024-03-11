#pragma once

#if AX_USE_X11

#include "../Base/axNativeUIApp_Base.h"
#include <ax_core/other/axObject.h>
#include <ax_core/container/axArray.h>

class axNativeUITimer;

class ax_X11_EventHandler : public axObject {
	const axUInt32 kSignature = 0xE011E011;
public:
	virtual void onExpose	() {}
	virtual void didDestroy	() {}
	
	virtual void onCloseButton() {}
	virtual void onActive	(bool b) {}

	virtual void onSetRect	(const axRect2f& rect) {}

	virtual void onKeyEvent		(XEvent& ev) {}
	virtual void onMouseEvent	(XEvent& ev) {}

	virtual void onCustomDestroyWindow() {}

			bool checkSignature() const { return _signature == kSignature; }

			void create(ax_X11_Display* dpy, ax_X11_Window win);

	bool _inExposeList = false;
private:
	axUInt32	_signature = kSignature;
};

class axNativeUIApp_X11 : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_X11, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	axNativeUIApp_X11();
	~axNativeUIApp_X11();

	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;

	ax_X11_XContext	xcontext()	{ return _xcontext; }
	ax_X11_Display*	xdisplay()	{ return _dpy; }	
	ax_X11_Window	rootWin()	{ return _rootWin; }

	ax_X11_XVisualInfo*	xvisualInfo()	{ return _xvisualInfo; }
	ax_X11_Colormap		colormap()		{ return _colormap; }


	static axRect2f	s_getVirtualDesktopRect();

	void _addHandlerNeedExpose(ax_X11_Window xwin, ax_X11_EventHandler& h);

	ax_X11_Status sendEvent_ClientMessage	(ax_X11_Window win, ax_X11_Atom msg, long p0=0, long p1=0, long p2=0, long p3=0, long p4=0);
	ax_X11_Status sendEvent_WMProtocol		(ax_X11_Window win, ax_X11_Atom subType,        long p1=0, long p2=0, long p3=0, long p4=0);

	ax_X11_XIM	xim() { return _xim; }

#define axNativeUIApp_X11_AtomList(E) \
		E(WM_PROTOCOLS) \
		E(WM_DELETE_WINDOW) \
		E(AX_CUSTOM_DESTROY_WINDOW) \
		E(AX_CUSTOM_APP_EVENT) \
		E(_MOTIF_WM_HINTS) \
		E(_NET_WM_WINDOW_TYPE) \
		E(_NET_WM_WINDOW_TYPE_DESKTOP) \
		E(_NET_WM_WINDOW_TYPE_DOCK) \
		E(_NET_WM_WINDOW_TYPE_TOOLBAR) \
		E(_NET_WM_WINDOW_TYPE_MENU) \
		E(_NET_WM_WINDOW_TYPE_UTILITY) \
		E(_NET_WM_WINDOW_TYPE_SPLASH) \
		E(_NET_WM_WINDOW_TYPE_DIALOG) \
		E(_NET_WM_WINDOW_TYPE_DROPDOWN_MENU) \
		E(_NET_WM_WINDOW_TYPE_POPUP_MENU) \
		E(_NET_WM_WINDOW_TYPE_TOOLTIP) \
		E(_NET_WM_WINDOW_TYPE_NOTIFICATION) \
		E(_NET_WM_WINDOW_TYPE_COMBO) \
		E(_NET_WM_WINDOW_TYPE_DND) \
		E(_NET_ACTIVE_WINDOW) \
		E(_NET_WM_ALLOWED_ACTIONS) \
		E(_NET_WM_ACTION_MOVE) \
		E(_NET_WM_ACTION_RESIZE) \
		E(_NET_WM_ACTION_MINIMIZE) \
		E(_NET_WM_ACTION_SHADE) \
		E(_NET_WM_ACTION_STICK) \
		E(_NET_WM_ACTION_MAXIMIZE_HORZ) \
		E(_NET_WM_ACTION_MAXIMIZE_VERT) \
		E(_NET_WM_ACTION_FULLSCREEN) \
		E(_NET_WM_ACTION_CHANGE_DESKTOP) \
		E(_NET_WM_ACTION_CLOSE) \
//---------

	struct AtomList : public axNonCopyable {
		void init(ax_X11_Display* dpy);
		#define E(T) ax_X11_Atom T = 0;
			axNativeUIApp_X11_AtomList(E)
		#undef E
	};
	AtomList _atoms;

	axArray<axNativeUITimer*, 64> _timers;

	#define AX_MACRO_OP(E, VALUE) unsigned int _cursor##E = 0;
		axUICursorType_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP

protected:
	void _os_run();
	virtual bool onRequestNativeCustomAppEvent() override;
	virtual bool onSetNativeCursor(axUICursorType type) override;
	virtual bool onSetNativeCursorPos(const axVec2f& worldPos) override;
	
private:
	void _os_init();
	void _os_release();

	static int _x11ErrorHandler(Display* dpy, XErrorEvent* err); 

	bool _timedWaitXEvent(struct timeval* tv);
	Time _nextTimerTimestamp();
	void _fireTimers();

	void _handleXEvent(XEvent& ev);
	axLinkArray<ax_X11_EventHandler>	_exposeList;

	ax_X11_XContext		_xcontext = 0;
	ax_X11_Display*		_dpy = nullptr;
	int					_fd = 0;
	bool				_shouldQuit {false};

	ax_X11_XVisualInfo*	_xvisualInfo = nullptr;
	ax_X11_Colormap		_colormap = 0;
	ax_X11_XIM			_xim = nullptr;
	ax_X11_Window		_rootWin = 0;
};

#endif // #if AX_USE_X11
