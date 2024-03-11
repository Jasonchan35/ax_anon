#pragma once

#include "axGLRenderContext_Base.h"

#if AX_USE_Gtk

class axGLRenderContext_Gtk : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_Gtk, axGLRenderContext_Base)
public:
	axGLRenderContext_Gtk(axGLRenderer* renderer, CreateDesc& desc);
	~axGLRenderContext_Gtk();

	virtual void cmdSwapBuffers() override;

	virtual void onMakeCurrent() override;
	virtual bool onIsCurrent() override;

	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;

	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axGLRenderContext_Gtk* ctx) {
		}
		
		~ScopedContext() {
		}		
	};

	void onX11_Expose() {}
	void onX11_Destroy() {}
	void onX11_DidDestroy() {}	

	class X11_EventHandler : public ax_Gtk_X11_EventHandler {
	public:
		X11_EventHandler(axGLRenderContext_Gtk* owner_) : owner(owner_) {}			
		axGLRenderContext_Gtk*	owner = nullptr;
	};
	
	X11_EventHandler	_x11_eventHandler;

	GtkGLArea*	_glarea = nullptr;
};

#endif // #if AX_USE_Gtk
