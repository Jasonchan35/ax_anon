#include "axGLRenderContext_Gtk.h"

#if AX_USE_Gtk
#if AX_USE_OPENGL

//ref: https://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib

GdkGLContext* axGLRenderContext_Gtk_on_create_context(GtkGLArea *area, axGLRenderContext_Gtk* thisObj) {
	GtkWindow* gtkWin = thisObj->_window->nativeWin();
	GdkWindow* gdkWin = gtk_widget_get_window(GTK_WIDGET(gtkWin));

	GError *error = NULL;
	auto* ctx = gdk_window_create_gl_context(gdkWin, &error);
	if (error != NULL) {
		throw axError_Undefined(AX_LOC);
		gtk_gl_area_set_error (area, error);
		g_clear_object (&ctx);
		g_clear_error(&error);
		return nullptr;
	}	
	return ctx;
}

axGLRenderContext_Gtk::axGLRenderContext_Gtk(axGLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
	, _x11_eventHandler(this)
{
	if (!window) {		
		// root context
		auto* win = GTK_WINDOW(gtk_window_new(GTK_WINDOW_POPUP));
		if (!win) throw axError_Undefined(AX_LOC);

		_glarea = GTK_GL_AREA(gtk_gl_area_new());
		if (!_glarea) throw axError_Undefined(AX_LOC);

		gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(_glarea));
		gtk_widget_realize(GTK_WIDGET(_glarea));
		makeCurrent();
		glewInit();

		// gtk_widget_show(GTK_WIDGET(win));
	} else {
		_glarea = GTK_GL_AREA(gtk_gl_area_new());
		if (!_glarea) throw axError_Undefined(AX_LOC);

		gtk_gl_area_set_auto_render(_glarea, false);

		g_signal_connect(_glarea, "create-context", G_CALLBACK(axGLRenderContext_Gtk_on_create_context), this);
		gtk_widget_realize(GTK_WIDGET(_glarea));
		makeCurrent();
		gtk_widget_show(GTK_WIDGET(_glarea));
	}
}

axGLRenderContext_Gtk::~axGLRenderContext_Gtk() {
	onPreDestroy();

}

void axGLRenderContext_Gtk::cmdSwapBuffers() {
	AX_ASSERT(isCurrent());
//	gtk_gl_area_swap_buffers(_glarea);
}

void axGLRenderContext_Gtk::onMakeCurrent() {
	gtk_gl_area_make_current(_glarea);
}

bool axGLRenderContext_Gtk::onIsCurrent() {
	auto* ctx = gtk_gl_area_get_context(_glarea);
	if (!ctx) return false;

	auto* cur = gdk_gl_context_get_current();
	return cur == ctx;
}
	
void axGLRenderContext_Gtk::onSetNativeViewRect(const axRect2f& rect) {
	// auto* w = GTK_WIDGET(_glarea);
	// auto* p = GTK_FIXED(gtk_widget_get_parent(w));
	// gtk_fixed_move(p, w, static_cast<gint>(rect.x), static_cast<gint>(rect.y));
	// gtk_widget_set_size_request(w, static_cast<gint>(rect.w), static_cast<gint>(rect.h));
}

void axGLRenderContext_Gtk::onSetNeedToRender() {
//	gdk_window_invalidate_rect(GDK_WINDOW(_win), nullptr, false);
}

#endif //AX_USE_OPENGL
#endif //AX_USE_Gtk

