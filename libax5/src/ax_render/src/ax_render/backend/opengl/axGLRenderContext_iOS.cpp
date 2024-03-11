//
//  axGLRenderContext_iOS.cpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#if AX_OS_IOS
#if AX_USE_OPENGL

#include "axGLRenderContext_iOS.h"

@interface axGLRenderContext_iOS_GLView : UIView {
	axGLRenderContext_iOS* _cpp;
}
@end

@implementation axGLRenderContext_iOS_GLView
+ (Class)layerClass {
	//this View has OpenGL layer
	return [CAEAGLLayer class];
}

-(instancetype) init:(axGLRenderContext_iOS*)cpp {
	self = [super init];
	_cpp = cpp;
	return self;
}

- (void)displayLayer:(CALayer*)layer {
	if (!_cpp) return;
	if (auto* p = _cpp->eventHandler()) {
		p->render(_cpp);
	}
}

- (void)drawRect:(CGRect)dirtyRect {
}
@end

axGLRenderContext_iOS::axGLRenderContext_iOS(axGLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	_view = [[axGLRenderContext_iOS_GLView alloc] init:this];
	if (!desc.window) {
		// root render context
//		_rootContextWin = [UIWindow new];
//		[_rootContextWin addSubview:_view];
	} else {
		[desc.window->rootView() addSubview: _view];
	}
	
	unsigned major, minor;
	EAGLGetVersion( &major, &minor );
//	AX_LOG( "OpenGL ES version({?},{?})", major, minor );
	
	auto *eaglLayer = (CAEAGLLayer *)[_view layer];
	
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO],
										kEAGLDrawablePropertyRetainedBacking, 
										kEAGLColorFormatRGBA8, 
										kEAGLDrawablePropertyColorFormat, nil];
	
	_ctx = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES3];
	if (!_ctx) throw axError_Undefined(AX_LOC);
	
	makeGLContextCurrent();
	_createBuffers();
}

axGLRenderContext_iOS::~axGLRenderContext_iOS() {
}

void axGLRenderContext_iOS::_createBuffers() {
	if (!_ctx) throw axError_Undefined(AX_LOC);

	_destroyBuffers();

	GLint backingWidth;
	GLint backingHeight;

	glGenFramebuffers(1, &_viewFramebuffer);
	glGenRenderbuffers(1, &_viewRenderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, _viewFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _viewRenderbuffer);
	[_ctx renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)[_view layer]];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _viewRenderbuffer);
	
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,  &backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

	if (backingWidth == 0 || backingHeight == 0)
		return;

	bool needDepthBuffer = true;
	if (needDepthBuffer) {
		glGenRenderbuffers(1, &_depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		AX_LOG("failed to make complete framebuffer object {?}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		throw axError_Undefined(AX_LOC);
	}
	
	glViewport (0, 0, backingWidth, backingHeight);
}

void axGLRenderContext_iOS::_destroyBuffers() {
	if (_viewFramebuffer) {
		glBindRenderbuffer(GL_RENDERBUFFER, _viewRenderbuffer);
		
		if (_viewRenderbuffer) {//detach
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
		}		
		if (_depthRenderbuffer) {//detach
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		}
		
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		glDeleteFramebuffers(1, &_viewFramebuffer);
		_viewFramebuffer = 0;
	}
	
	if (_viewRenderbuffer) {
		glDeleteRenderbuffers(1, &_viewRenderbuffer);
		_viewRenderbuffer = 0;
    }
	
	if (_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &_depthRenderbuffer);
		_depthRenderbuffer = 0;
	}
}

void axGLRenderContext_iOS::onMakeGLContextCurrent() {
	[EAGLContext setCurrentContext:_ctx];
}

bool axGLRenderContext_iOS::onIsGLContextCurrent() {
	auto* c = [EAGLContext currentContext];
	return c == _ctx;
}

void axGLRenderContext_iOS::cmdSwapBuffers() {
	glBindRenderbuffer(GL_RENDERBUFFER, _viewRenderbuffer);
	[_ctx presentRenderbuffer:GL_RENDERBUFFER];
}

void axGLRenderContext_iOS::onSetNeedToRender() {
	[_view setNeedsDisplay];
}

void axGLRenderContext_iOS::onSetNativeViewRect(const axRect2f &rect) {
	if (_view) {
		auto h = (float)[_view superview].frame.size.height;
		auto rc = rect;
		rc.y = h - rect.yMax();
		[_view setFrame:rc];
		_createBuffers();
	}
	Base::onSetNativeViewRect(rect);
}

#endif //AX_USE_OPENGL
#endif //AX_OS_IOS
