#pragma once

//
//  axGLRenderContext_X11.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axGLRenderContext_Base.h"

#if AX_USE_X11
#if AX_USE_OPENGL

class axGLRenderContext_X11 : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_X11, axGLRenderContext_Base)
public:
	axGLRenderContext_X11(axGLRenderer* renderer, CreateDesc& desc);
	virtual ~axGLRenderContext_X11();

	virtual void cmdSwapBuffers	() override;

friend class axGLRenderContext_X11_Scope;
protected:
	virtual void onMakeGLContextCurrent() override;
	virtual void onUnmakeGLContextCurrent() override;
	virtual bool onIsGLContextCurrent	() override;	
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;

	class FalseContext;

	void onX11_Expose();
	void onX11_Destroy();
	void onX11_DidDestroy();

	class X11_EventHandler : public ax_X11_EventHandler {
	public:
		X11_EventHandler(axGLRenderContext_X11* owner_) : owner(owner_) {}

		virtual void onExpose	() override { if(owner) owner->onX11_Expose(); }
		virtual void didDestroy	() override { if(owner) owner->onX11_DidDestroy(); }

		virtual void onSetRect	(const axRect2f& rect) override {}

		virtual void onKeyEvent		(XEvent& ev) override { if(owner) owner->_window->_handleNativeUIKeyEvent(ev);   }
		virtual void onMouseEvent	(XEvent& ev) override { if(owner) owner->_window->_handleNativeUIMouseEvent(ev); }
				
		axGLRenderContext_X11*	owner = nullptr;
	};
	
	X11_EventHandler	_x11_eventHandler;
	ax_X11_GLXContext	_glcontext = nullptr;
	ax_X11_Window		_win = 0;
	ax_X11_Display*		_dpy = nullptr;
	bool _lastVSync = true;
};

#endif //AX_USE_OPENGL
#endif //AX_USE_X11
