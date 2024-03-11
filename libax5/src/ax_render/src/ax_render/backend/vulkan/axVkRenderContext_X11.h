#pragma once

//
//  axVkRenderContext_X11.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axVkRenderContext_Base.h"

#if AX_USE_X11
#if AX_USE_VULKAN

class axVkRenderContext_X11 : public axVkRenderContext_Base {
	AX_RTTI_CLASS(axVkRenderContext_X11, axVkRenderContext_Base)
public:
	axVkRenderContext_X11(axVkRenderer* renderer, CreateDesc& desc);
	virtual ~axVkRenderContext_X11();

protected:
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() final;
	virtual void onCreateVkSurface(axCppVkSurfaceKHR& surface) final;

	void onX11_Expose();
	void onX11_Destroy();
	void onX11_DidDestroy();

	class X11_EventHandler : public ax_X11_EventHandler {
	public:
		X11_EventHandler(axVkRenderContext_X11* owner_) : owner(owner_) {}

		virtual void onExpose	() override { if(owner) owner->onX11_Expose(); }
		virtual void didDestroy	() override { if(owner) owner->onX11_DidDestroy(); }

		virtual void onSetRect	(const axRect2f& rect) override {}

		virtual void onKeyEvent		(XEvent& ev) override { if(owner) owner->_window->_handleNativeUIKeyEvent(ev);   }
		virtual void onMouseEvent	(XEvent& ev) override { if(owner) owner->_window->_handleNativeUIMouseEvent(ev); }
				
		axVkRenderContext_X11*	owner = nullptr;
	};
	
	X11_EventHandler	_x11_eventHandler;
	ax_X11_Window		_win = 0;
	ax_X11_Display*		_dpy = nullptr;
};

#endif //AX_USE_VULKAN
#endif //AX_USE_X11
