//
//  axGLRenderContext_MacOSX.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axGLRenderContext.h"

#if AX_OS_OSX
#if AX_USE_OPENGL

@interface axGLRenderContext_MacOSX_GLView : NSOpenGLView {
	axGLRenderContext_MacOSX* _cpp;
	NSTrackingArea* _ta;
}
- (void)setCpp:(axGLRenderContext_MacOSX*)cpp;
@end

@implementation axGLRenderContext_MacOSX_GLView
- (void)setCpp:(axGLRenderContext_MacOSX*)p {
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

- (void)drawRect:(NSRect)dirtyRect {
	if (!_cpp) return;
	if (auto* p = _cpp->eventHandler()) {
		p->render(_cpp);
	}
}

- (void)update {
	[super update];
	[self setNeedsDisplay:true];
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

- (void)reshape {
	[self setNeedsDisplay:true];
	[super reshape];
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

axGLRenderContext_MacOSX::axGLRenderContext_MacOSX(axGLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
		//	NSOpenGLPFAColorFloat,
		NSOpenGLPFADoubleBuffer,
		//	NSOpenGLPFATripleBuffer,
		NSOpenGLPFAColorSize,	32,
		NSOpenGLPFAAlphaSize,	8,
		NSOpenGLPFADepthSize,	24,
		NSOpenGLPFAStencilSize, 8,
		0
	};
	
	auto fmt = axNSObj_make([[NSOpenGLPixelFormat alloc] initWithAttributes:attrs]);
	if (!fmt) throw axError_Undefined(AX_LOC);

	bool isRoot = false;
	{
		axGLRenderer::ScopedRootContext scopedRootContext;

		auto* rootCtx = scopedRootContext.rootContext();
		NSOpenGLContext* shareContext = rootCtx ? rootCtx->_ctx.ptr() : nil;
		if (!shareContext) {
			isRoot = true;
		}

	//	auto fmt = [NSOpenGLView defaultPixelFormat];
		_ctx.ref([[NSOpenGLContext alloc] initWithFormat:fmt shareContext:shareContext]);
		if (!_ctx) throw axError_Undefined(AX_LOC);
	}

	ScopedContext scopedContext(this);

	//--- add to window content view
	if (isRoot) {
		glewInit();
	}

	if (desc.window) {
		auto* gv =[axGLRenderContext_MacOSX_GLView alloc];
		if (!gv) throw axError_Undefined(AX_LOC);
		[gv setCpp:this];

		_gv.ref([gv initWithFrame:axRect2f(0,0,0,0)]);
		if (!_gv) throw axError_Undefined(AX_LOC);

		[_ctx setView:gv];
		[gv setOpenGLContext:_ctx];
		[gv onCreate];

		auto* cv = desc.window->contentView();
		AX_ASSERT(cv);

		[gv setWantsBestResolutionOpenGLSurface: false];

		[cv addSubview: gv];
	}
}

axGLRenderContext_MacOSX::~axGLRenderContext_MacOSX() {
	onPreDestroy();
	if (_gv) {
		[_gv removeFromSuperview];
		[_gv setCpp:nullptr];
		[_gv setOpenGLContext:nullptr];
		_gv = nullptr;
	}
	if (_ctx) {
		[_ctx setView: nullptr];
		_ctx = nullptr;
	}
}

void axGLRenderContext_MacOSX::cmdSwapBuffers() {
	AX_ASSERT(_ctx);
	
	auto vsync = _renderer->vsync();
	if (_lastVSync != vsync) {
		GLint v = vsync ? 1 : 0;
		[_ctx setValues:&v forParameter:NSOpenGLCPSwapInterval];
		_lastVSync = vsync;
	}
	[_ctx flushBuffer];
}

void axGLRenderContext_MacOSX::onMakeGLContextCurrent() {
	AX_ASSERT(_ctx);
	[_ctx makeCurrentContext];
}

void axGLRenderContext_MacOSX::onUnmakeGLContextCurrent() {
	[NSOpenGLContext clearCurrentContext];
}

bool axGLRenderContext_MacOSX::onIsGLContextCurrent() {
	if (!_ctx) return false;
	return _ctx == [NSOpenGLContext currentContext];
}

void axGLRenderContext_MacOSX::onSetNativeViewRect(const axRect2f& rect) {
	if (_gv) {
		auto h = (float)[_gv superview].frame.size.height;
		auto rc = rect;
		rc.y = h - rect.yMax();
		[_gv setFrame:rc];
	}
	Base::onSetNativeViewRect(rect);
}

void axGLRenderContext_MacOSX::onSetNeedToRender() {
	if (_gv) {
		[_gv setNeedsDisplay:true];
	}
}

void axGLRenderContext_MacOSX::_handleMouseEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIMouseEvent(inEvent);
}

void axGLRenderContext_MacOSX::_handleKeyEvent(NSEvent* inEvent) {
	if (!_window) return;
	_window->_handleNativeUIKeyEvent(inEvent);
}

#endif //AX_USE_OPENGL
#endif //AX_OS_OSX
