//
//  axMTLRenderContext_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_IOS
#if AX_USE_METAL

#include "axMTLRenderContext_Base.h"

@class axMTLRenderContext_iOS_GLView;

class axMTLRenderContext_iOS : public axMTLRenderContext_Base {
	AX_RTTI_CLASS(axMTLRenderContext_iOS, axMTLRenderContext_Base)
public:
	axMTLRenderContext_iOS(axMTLRenderer* renderer, CreateDesc& desc);
	~axMTLRenderContext_iOS();
	
	virtual void cmdSwapBuffers() override;
	virtual void onMakeCurrent() override;
	virtual bool onIsCurrent() override;
	
	virtual void setNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axMTLRenderContext_iOS* ctx) {
			auto p = [EAGLContext currentContext];
			if (p != ctx->_ctx) {
				_prev.ref(p);
				ctx->makeCurrent();
			}
		}
		
		~ScopedContext() {
			if (_prev) [EAGLContext setCurrentContext:_prev];
		}
		
		axNSObj<EAGLContext> _prev;
	};
	
private:
	void _createBuffers();
	void _destroyBuffers();

	axNSObj<EAGLContext>					_ctx;
	axNSObj<UIWindow>						_rootContextWin;
	axNSObj<axMTLRenderContext_iOS_GLView>	_view;
	
    GLuint  _viewRenderbuffer = 0;
    GLuint  _viewFramebuffer = 0;
    GLuint  _depthRenderbuffer = 0;
};

#endif //AX_USE_METAL
#endif // AX_OS_IOS
