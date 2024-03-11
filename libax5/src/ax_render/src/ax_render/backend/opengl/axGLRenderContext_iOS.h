//
//  axGLRenderContext_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_IOS
#if AX_USE_OPENGL

#include "axGLRenderContext_Base.h"

@class axGLRenderContext_iOS_GLView;

class axGLRenderContext_iOS : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_iOS, axGLRenderContext_Base)
public:

	struct NativeContext {
		axNSObj<EAGLContext> ctx;
	};

	axGLRenderContext_iOS(axGLRenderer* renderer, CreateDesc& desc);
	~axGLRenderContext_iOS();
	
	virtual void cmdSwapBuffers() override;
	virtual void onMakeGLContextCurrent() override;
	virtual bool onIsGLContextCurrent() override;
	
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	
	/*
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axGLRenderContext_iOS* ctx) {
			auto p = [EAGLContext currentContext];
			if (p != ctx->_ctx) {
				_prev.ref(p);
				ctx->makeGLContextCurrent();
			}
		}
		
		~ScopedContext() {
			if (_prev) [EAGLContext setCurrentContext:_prev];
		}
		
		axNSObj<EAGLContext> _prev;
	};
	*/
	
private:
	void _createBuffers();
	void _destroyBuffers();

	axNSObj<UIWindow>						_rootContextWin;
	axNSObj<axGLRenderContext_iOS_GLView>	_view;
	NativeContext _ctx;
	
	GLuint	_viewRenderbuffer = 0;
	GLuint	_viewFramebuffer = 0;
	GLuint	_depthRenderbuffer = 0;

};

#endif //AX_USE_OPENGL
#endif // AX_OS_IOS
