//
//  axMTLRenderContext_MacOSX.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#pragma once

#include "axMTLRenderContext_Base.h"

#if AX_OS_OSX
#if AX_USE_METAL

@class axMTLRenderContext_MacOSX_MTKView;

class axMTLRenderContext_MacOSX : public axMTLRenderContext_Base {
	AX_RTTI_CLASS(axMTLRenderContext_MacOSX, axMTLRenderContext_Base)
public:
	axMTLRenderContext_MacOSX(axMTLRenderer* renderer, CreateDesc& desc);
	~axMTLRenderContext_MacOSX();
		
	void _handleMouseEvent	(NSEvent* inEvent);
	void _handleKeyEvent	(NSEvent* inEvent);
		
	axNSObj<axMTLRenderContext_MacOSX_MTKView> _macMtkView;
};

#endif //AX_USE_METAL
#endif //AX_OS_OSX
