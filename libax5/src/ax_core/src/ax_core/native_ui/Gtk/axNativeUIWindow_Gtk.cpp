#if AX_USE_Gtk
#include "axNativeUIWindow_Gtk.h"

static void axNativeUIWindow_Gtk_on_destroy(GtkWidget *widget, axNativeUIWindow_Gtk* thisObj) {
	thisObj->onWindowDestroyed();
}

static gboolean axNativeUIWindow_Gtk_on_event(GtkWidget *widget, GdkEvent* inEvent, axNativeUIWindow_Gtk* thisObj) {
	switch (inEvent->type) {
		case GDK_DELETE: {
			if (thisObj->onShouldClose())
				gtk_widget_destroy(widget);
			return true;
		}break;

		case GDK_CONFIGURE: {
			auto& ev = inEvent->configure;
			thisObj->onSetWorldPos(axVec2f(static_cast<float>(ev.x),     static_cast<float>(ev.y)));
			thisObj->onSetSize    (axVec2f(static_cast<float>(ev.width), static_cast<float>(ev.height)));			
		}break;
	}
	return false;
}

axNativeUIWindow_Gtk::axNativeUIWindow_Gtk(CreateDesc& desc) 
	: Base(desc)
{
	// auto* app = axNativeUIApp::s_instance();
	// auto& quarks = app->_quarks;

	GtkWindowType winType = GTK_WINDOW_TOPLEVEL;

	switch (desc.type) {
		case Desc::Type::ToolWindow:
		case Desc::Type::NormalWindow: {
			// dwStyle   |= WS_OVERLAPPED | WS_SYSMENU;

			// if (desc.closeButton) dwStyle |= WS_SYSMENU;
			// if (desc.resizable  ) dwStyle |= WS_THICKFRAME;
			// if (desc.titleBar   ) dwStyle |= WS_CAPTION;
			// if (desc.minButton  ) dwStyle |= WS_MINIMIZEBOX;
			// if (desc.maxButton  ) dwStyle |= WS_MAXIMIZEBOX;
		}break;

		case Desc::Type::PopupWindow: {
			winType = GTK_WINDOW_POPUP;
		}break;
		default: AX_ASSERT(false); break;
	}

	_win = GTK_WINDOW(gtk_window_new(winType));
	if (!_win) {
		throw axError_Undefined(AX_LOC);
	}

//	gtk_widget_set_events(GTK_WIDGET(_win), GDK_EXPOSURE_MASK);

	g_signal_connect(_win, "event",         G_CALLBACK(axNativeUIWindow_Gtk_on_event),   this);
	g_signal_connect(_win, "destroy",       G_CALLBACK(axNativeUIWindow_Gtk_on_destroy), this);

	gtk_widget_realize(GTK_WIDGET(_win));

	_rootWidget = GTK_FIXED(gtk_fixed_new());
	if (!_rootWidget) {
		throw axError_Undefined(AX_LOC);
	}
}

axNativeUIWindow_Gtk::~axNativeUIWindow_Gtk() {
}

void axNativeUIWindow_Gtk::onCreate(Desc& desc) {
}

void axNativeUIWindow_Gtk::onSetNativeActive(bool b) {
}

void axNativeUIWindow_Gtk::onSetNativeVisible(bool b) {
	if (b) {
		gtk_widget_show(GTK_WIDGET(_win));
	} else {
		gtk_widget_hide(GTK_WIDGET(_win));
	}
}

void axNativeUIWindow_Gtk::onSetNativeWindowTitle(axStrView title) {
	axTempString tmp(title);
	gtk_window_set_title(_win, tmp.c_str());
}

void axNativeUIWindow_Gtk::onSetNativeWorldPos(const axVec2f& v) {
	gtk_window_move(_win, static_cast<gint>(v.x), static_cast<gint>(v.y));
}

void axNativeUIWindow_Gtk::onSetNativeSize(const axVec2f& v) {
	gtk_window_resize(_win, static_cast<gint>(v.x), static_cast<gint>(v.y));
}

#endif
