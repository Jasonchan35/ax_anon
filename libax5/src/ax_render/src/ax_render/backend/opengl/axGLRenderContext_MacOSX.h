//
//  axGLRenderContext_MacOSX.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#pragma once

#include "axGLRenderContext_Base.h"

#if AX_OS_OSX
#if AX_USE_OPENGL

@class axGLRenderContext_MacOSX_GLView;

class axGLRenderContext_MacOSX : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_MacOSX, axGLRenderContext_Base)
public:

	axGLRenderContext_MacOSX(axGLRenderer* renderer, CreateDesc& desc);
	~axGLRenderContext_MacOSX();
		
	virtual void cmdSwapBuffers() override;

	virtual void onMakeGLContextCurrent() override;
	virtual void onUnmakeGLContextCurrent() override;
	virtual bool onIsGLContextCurrent() override;

	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;

	void _handleMouseEvent	(NSEvent* inEvent);
	void _handleKeyEvent	(NSEvent* inEvent);

	/*
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axGLRenderContext_MacOSX* ctx) {
			auto p = [NSOpenGLContext currentContext];
			if (p != ctx->_ctx) {
				_prev.ref(p);
				ctx->makeGLContextCurrent();
			}
		}
		
		~ScopedContext() {
			if (_prev) [_prev makeCurrentContext];
		}
		
		axNSObj<NSOpenGLContext> _prev;
	};
	*/
		
	axNSObj<axGLRenderContext_MacOSX_GLView> _gv;
	axNSObj<NSOpenGLContext> _ctx;
	bool _lastVSync = true;
};

#endif //AX_USE_OPENGL
#endif //AX_OS_OSX
