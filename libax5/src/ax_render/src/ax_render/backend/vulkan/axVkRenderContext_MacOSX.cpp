//
//  axVkRenderContext_MacOSX.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axVkRenderContext_MacOSX.h"

#if AX_OS_OSX
#if AX_USE_VULKAN

@interface axVkRenderContext_MacOSX_NSView : MTKView {
	axVkRenderContext_MacOSX* _cpp;
	NSTrackingArea* _ta;
//	CAMetalLayer* _metalLayer;
}
- (void)setCpp:(axVkRenderContext_MacOSX*)cpp;
@end

@implementation axVkRenderContext_MacOSX_NSView
- (void)setCpp:(axVkRenderContext_MacOSX*)p {
	_cpp = p;
}
- (BOOL)isFlipped 				{ return true; }

- (BOOL)acceptsFirstResponder 	{ return true; }
//- (BOOL)becomeFirstResponder 	{ return true; }
//- (BOOL)resignFirstResponder 	{ return true; }

- (void)onCreate {
	[self addTrackingArea:_ta];
	self.wantsLayer = YES;
//	_metalLayer = [CAMetalLayer layer];
//	self.layer = _metalLayer;
}
//- (CAMetalLayer*)metalLayer { return _metalLayer; }

- (void)doRender {
	if (!_cpp) return;
	if (auto* p = _cpp->eventHandler()) {
		p->render(_cpp);
	}
}

- (void)drawRect:(NSRect)dirtyRect {
	if (!_cpp) return;
	[self doRender];
}

//- (BOOL)wantsUpdateLayer { return YES; }
//- (void)updateLayer { [self setNeedsDisplay:true]; }

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

axVkRenderContext_MacOSX::axVkRenderContext_MacOSX(axVkRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	auto* view =[axVkRenderContext_MacOSX_NSView alloc];
	if (!view) throw axError_Undefined(AX_LOC);
	[view setCpp:this];

	_nsView.ref([view initWithFrame:axRect2f(0,0,100,100)]);
	if (!_nsView) throw axError_Undefined(AX_LOC);

	[_nsView setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];
	[_nsView onCreate];

	if (desc.window) {
		auto* cv = desc.window->contentView();
		AX_ASSERT(cv);
		[cv addSubview: _nsView];
	}

	_createDevice();
}

axVkRenderContext_MacOSX::~axVkRenderContext_MacOSX() {
	onPreDestroy();
	if (_nsView) {
		[_nsView removeFromSuperview];
	}
}

void axVkRenderContext_MacOSX::onSetNativeViewRect(const axRect2f& rect) {
	if (_nsView) {
		auto h = (float)[_nsView superview].frame.size.height;
		auto rc = rect;
		rc.y = h - rect.yMax();
		[_nsView setFrame:rc];
		[_nsView setDrawableSize:rc.size];
	}
	Base::onSetNativeViewRect(rect);
}

void axVkRenderContext_MacOSX::onSetNeedToRender() {
	if (_nsView) {
		auto size = _nsView.ptr().bounds.size;
		[_nsView setDrawableSize:size];
		[_nsView setNeedsDisplay:true];
	}
}

void axVkRenderContext_MacOSX::onCreateVkSurface(axCppVkSurfaceKHR& surface) {
	auto& device = renderer()->device();
	surface.create_macOS(*device.physicalDevice(), _nsView);
}

void axVkRenderContext_MacOSX::_handleMouseEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIMouseEvent(inEvent);
}

void axVkRenderContext_MacOSX::_handleKeyEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIKeyEvent(inEvent);
}

#endif //AX_USE_VULKAN
#endif //AX_OS_OSX
