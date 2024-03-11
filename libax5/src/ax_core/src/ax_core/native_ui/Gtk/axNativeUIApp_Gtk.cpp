#if AX_USE_Gtk
#include "axNativeUIApp_Gtk.h"

axNativeUIApp_Gtk::axNativeUIApp_Gtk() {
	_xcontext = XUniqueContext();
	if (!_xcontext) throw axError_Undefined(AX_LOC);

	_dpy = XOpenDisplay(nullptr);
	if (!_dpy) throw axError_Undefined(AX_LOC);

	#define ax_op(E) \
		_quarks.E = g_quark_from_static_string(#E);
	//-----
		ax_op(ax_userdata);
	#undef ax_op
}

int axNativeUIApp_Gtk::onRun() {
	create();
	gtk_main();
	willQuit();
	return _returnCode;
}

void axNativeUIApp_Gtk::quit(int returnCode) {
	_returnCode = returnCode;
	gtk_main_quit();
}

axVec2f axNativeUIApp_Gtk::s_getVirtualDesktopSize() {
	auto* dpy = gdk_display_get_default();
	auto* mon = gdk_display_get_primary_monitor(dpy);

	GdkRectangle rect;
	gdk_monitor_get_workarea(mon, &rect);

	return axVec2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
}

#endif //AX_USE_Gtk
