//
//  axVkRenderContext_MacOSX.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#pragma once

#include "axVkRenderContext_Base.h"

#if AX_OS_OSX
#if AX_USE_VULKAN

@class axVkRenderContext_MacOSX_NSView;

class axVkRenderContext_MacOSX : public axVkRenderContext_Base {
	AX_RTTI_CLASS(axVkRenderContext_MacOSX, axVkRenderContext_Base)
public:
	axVkRenderContext_MacOSX(axVkRenderer* renderer, CreateDesc& desc);
	~axVkRenderContext_MacOSX();
		
	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() final;
	virtual void onCreateVkSurface(axCppVkSurfaceKHR& surface) final;

	void _handleMouseEvent	(NSEvent* inEvent);
	void _handleKeyEvent	(NSEvent* inEvent);

	axNSObj<axVkRenderContext_MacOSX_NSView> _nsView;
};

#endif //AX_USE_VULKAN
#endif //AX_OS_OSX
