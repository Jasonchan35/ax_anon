//
//  axMTLRenderContext_MacOSX.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axMTLRenderContext.h"

#if AX_OS_OSX
#if AX_USE_METAL

@interface axMTLRenderContext_MacOSX_MTKView : MTKView {
	axMTLRenderContext_MacOSX* _cpp;
	NSTrackingArea* _ta;
}
- (void)setCpp:(axMTLRenderContext_MacOSX*)cpp;
@end

@implementation axMTLRenderContext_MacOSX_MTKView
- (void)setCpp:(axMTLRenderContext_MacOSX*)p {
	_cpp = p;
}
- (BOOL)isFlipped 				{ return true; }

- (BOOL)acceptsFirstResponder 	{ return true; }
//- (BOOL)becomeFirstResponder 	{ return true; }
//- (BOOL)resignFirstResponder 	{ return true; }

- (void)onCreate {
	[self setAllowedTouchTypes:NSTouchTypeMaskDirect | NSTouchTypeMaskIndirect];
	[self addTrackingArea:_ta];
}

- (void)doRender {
	if (!_cpp) return;
	if (auto* p = _cpp->eventHandler()) {
		p->render(_cpp);
	}
}

- (void)drawRect:(CGRect)rect {
	[super drawRect:rect];
	[self doRender];
}

- (void) updateTrackingAreas {
	if (_ta) {
		[self removeTrackingArea:_ta];
	}

	_ta = [[NSTrackingArea alloc]	initWithRect:self.bounds
									options:NSTrackingMouseMoved | NSTrackingActiveWhenFirstResponder
									owner:self
									userInfo:nil];
	[self addTrackingArea:_ta];
	[super updateTrackingAreas];
}

//- (void)reshape {
//	[self setNeedsDisplay:true];
//	[super reshape];
//}

- (void)mouseMoved:			(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)rightMouseMoved:	(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)otherMouseMoved:	(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }

- (void)mouseDragged:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)rightMouseDragged:	(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)otherMouseDragged:	(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }

- (void)mouseDown:			(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)rightMouseDown:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)otherMouseDown:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }

- (void)mouseUp:			(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)rightMouseUp:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }
- (void)otherMouseUp:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }

- (void)scrollWheel:		(NSEvent *)ev { if (_cpp) _cpp->_handleMouseEvent(ev); }

//- (void) tabletPoint:		(NSEvent *)ev {}
//- (void) tabletProximity:	(NSEvent *)ev {}

//- (void)flagsChanged:		(NSEvent *)ev {}
- (void)keyDown:			(NSEvent *)ev { if (_cpp) _cpp->_handleKeyEvent(ev); }
- (void)keyUp:				(NSEvent *)ev { if (_cpp) _cpp->_handleKeyEvent(ev); }


@end

axMTLRenderContext_MacOSX::axMTLRenderContext_MacOSX(axMTLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	auto* view =[axMTLRenderContext_MacOSX_MTKView alloc];
	if (!view) throw axError_Undefined(AX_LOC);
	[view setCpp:this];

	auto mtlDevice = renderer->mtlDevice();

	_macMtkView.ref([view initWithFrame:axRect2f(0,0,32,32) device:mtlDevice]);
	if (!_macMtkView) throw axError_Undefined(AX_LOC);

	[_macMtkView setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];
	[_macMtkView onCreate];

	if (desc.window) {
		auto* cv = desc.window->contentView();
		AX_ASSERT(cv);
		[cv addSubview: _macMtkView];
	}

	_onCreate(_macMtkView);
}

axMTLRenderContext_MacOSX::~axMTLRenderContext_MacOSX() {
	onPreDestroy();
	if (_macMtkView) {
		[_macMtkView removeFromSuperview];
	}
}

void axMTLRenderContext_MacOSX::_handleMouseEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIMouseEvent(inEvent);
}

void axMTLRenderContext_MacOSX::_handleKeyEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIKeyEvent(inEvent);
}

#endif //AX_USE_METAL
#endif //AX_OS_OSX
