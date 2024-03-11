//
//  axGLRenderContext_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_ANDROID
#if AX_USE_OPENGL

#include "axGLRenderContext_Base.h"

class axGLRenderContext_Android : public axGLRenderContext_Base {
	AX_RTTI_CLASS(axGLRenderContext_Android, axGLRenderContext_Base)
public:
	axGLRenderContext_Android(axGLRenderer* renderer, CreateDesc& desc);
	~axGLRenderContext_Android();
	
	virtual void cmdSwapBuffers() override;
	virtual void onMakeGLContextCurrent() override;
	virtual void onUnmakeGLContextCurrent() override;
	

	virtual bool onIsGLContextCurrent() override;
	
	virtual void setNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axGLRenderContext_Android* ctx) {
			/*
			auto p = [EAGLContext currentContext];
			if (p != ctx->_ctx) {
				_prev.ref(p);
				ctx->makeCurrent();
			}
			*/
		}
		
		~ScopedContext() {
			//if (_prev) [EAGLContext setCurrentContext:_prev];
		}
		
		//axNSObj<EAGLContext> _prev;
	};	
};

#endif // AX_USE_OPENGL
#endif // AX_OS_ANDROID
