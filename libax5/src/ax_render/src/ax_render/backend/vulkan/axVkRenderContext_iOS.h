//
//  axVkRenderContext_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_IOS
#if AX_USE_VULKAN

#include "axVkRenderContext_Base.h"

@class axVkRenderContext_iOS_GLView;

class axVkRenderContext_iOS : public axVkRenderContext_Base {
	AX_RTTI_CLASS(axVkRenderContext_iOS, axVkRenderContext_Base)
public:
	axVkRenderContext_iOS(axVkRenderer* renderer, CreateDesc& desc);
	~axVkRenderContext_iOS();
		
	virtual void setNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axVkRenderContext_iOS* ctx) {
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
	axNSObj<axVkRenderContext_iOS_GLView>	_view;
	
    GLuint  _viewRenderbuffer = 0;
    GLuint  _viewFramebuffer = 0;
    GLuint  _depthRenderbuffer = 0;
};

#endif //AX_USE_VULKAN
#endif // AX_OS_IOS
