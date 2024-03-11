//
//  axMTLRenderContext_iOS.cpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#if AX_OS_IOS
#if AX_USE_METAL

#include "axMTLRenderContext_iOS.h"

@interface axMTLRenderContext_iOS_GLView : UIView {
	axMTLRenderContext_iOS* _cpp;
}
@end

@implementation axMTLRenderContext_iOS_GLView
+ (Class)layerClass {
	//this View has OpenGL layer
    return [CAEAGLLayer class];
}

-(instancetype) init:(axMTLRenderContext_iOS*)cpp {
	self = [super init];
	_cpp = cpp;
	return self;
}
@end

axMTLRenderContext_iOS::axMTLRenderContext_iOS(axMTLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	ax_Metal_TODO
}

axMTLRenderContext_iOS::~axMTLRenderContext_iOS() {
}

void axMTLRenderContext_iOS::_createBuffers() {
	ax_Metal_TODO
}

void axMTLRenderContext_iOS::_destroyBuffers() {
	ax_Metal_TODO
}

void axMTLRenderContext_iOS::onMakeCurrent() {
	ax_Metal_TODO
}

bool axMTLRenderContext_iOS::onIsCurrent() {
	ax_Metal_TODO
	return true;
}

void axMTLRenderContext_iOS::cmdSwapBuffers() {
	ax_Metal_TODO
}

void axMTLRenderContext_iOS::onSetNeedToRender() {
	ax_Metal_TODO
}

void axMTLRenderContext_iOS::setNativeViewRect(const axRect2f &rect) {
	ax_Metal_TODO
}

#endif //AX_USE_METAL
#endif //AX_OS_IOS
