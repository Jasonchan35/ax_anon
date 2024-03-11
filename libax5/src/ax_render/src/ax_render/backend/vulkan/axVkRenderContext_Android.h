//
//  axVkRenderContext_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_ANDROID
#if AX_USE_VULKAN

#include "axVkRenderContext_Base.h"

class axVkRenderContext_Android : public axVkRenderContext_Base {
	AX_RTTI_CLASS(axVkRenderContext_Android, axVkRenderContext_Base)
public:
	axVkRenderContext_Android(axVkRenderer* renderer, CreateDesc& desc);
	~axVkRenderContext_Android();
	
	virtual void cmdSwapBuffers() override;
	virtual void onMakeCurrent() override;
	virtual bool onIsCurrent() override;
	
	virtual void setNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;
	
	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axVkRenderContext_Android* ctx) {
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

#endif // AX_USE_VULKAN
#endif // AX_OS_ANDROID
