#pragma once

#if AX_USE_Gtk
#include "../Base/axNativeUIApp_Base.h"

class ax_Gtk_X11_EventHandler : public axNonCopyable {
public:
	void onEvent(XEvent& ev) {}
	bool checkSignature() { return true; }
};

class axNativeUIApp_Gtk : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_Gtk, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;

	static axVec2f s_getVirtualDesktopSize();

	struct Quarks {
		GQuark ax_userdata;
	};

	Quarks _quarks;

	axNativeUIApp_Gtk();

	ax_X11_XContext	xcontext()	{ return _xcontext; }

private:
	void _os_run();

	ax_X11_Display*		_dpy = nullptr;
	ax_X11_XContext		_xcontext = 0;
};

#endif
