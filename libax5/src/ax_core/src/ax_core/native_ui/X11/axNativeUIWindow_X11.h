#pragma once

#if AX_USE_X11
#include "../Base/axNativeUIWindow_Base.h"
#include "axNativeUIApp_X11.h"
#include "../axNativeUIScreen.h"

class axNativeUIWindow_X11 : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_X11, axNativeUIWindow_Base);
public:
	axNativeUIWindow_X11(CreateDesc & desc);
	virtual ~axNativeUIWindow_X11();

	class EventHandler : public ax_X11_EventHandler {
	public:
		virtual void onExpose	() override;
		virtual void didDestroy	() override;
		virtual void onCloseButton() override;
		virtual void onCustomDestroyWindow() override;
		virtual void onActive	(bool b) override;
		virtual void onSetRect	(const axRect2f& rect) override;

		virtual void onMouseEvent	(XEvent& ev) override;
		virtual void onKeyEvent		(XEvent& ev) override;
		
		axNativeUIWindow_X11*	owner {nullptr};		
	};

	void _handleNativeUIMouseEvent(XEvent& inEvent);
	void _handleNativeUIKeyEvent(XEvent& inEvent);
	void _destroy();

	axNativeUIScreen	screen();

	ax_X11_Window	xwindow() { return _win; }
	ax_X11_XIC		x11_xic() { return _xic; }

friend class EventHandler;
protected:
	virtual void onSetNativeActive	(bool b) override;
	virtual void onSetNativeVisible	(bool b) override;
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
	virtual void doCloseWindow() override;
	virtual void doDestroyWindow() override;

	bool _handling_XEvent_setPos : 1;
	bool _handling_XEvent_setSize: 1;

	EventHandler _eventHandler;

	void _updateMousePressedButtons	(unsigned int xEventState);
	void _updateEventModifier		(unsigned int xEventState);

	axNativeUIEventModifier		_eventModifier;
	axNativeUIMouseEventButton	_mousePressedButtons;
	axNativeUIMouseEventButton	_lastMousePressedButtons;

	bool		_setX11WinProperty		(ax_X11_Atom prop, axSpan<ax_X11_Atom> values, bool printError = true);

	Atom		_getX11Atom(axStrLiteral name);

	Window	 	_getX11ParentWindow() const;
	axRect2f	_getX11WorldRect() const;

	ax_X11_Display*		_dpy = nullptr;
	ax_X11_Window		_win = 0;
	ax_X11_XIC			_xic = nullptr;
};

#endif //#if AX_USE_X11
