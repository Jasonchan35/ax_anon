//
//  axVkRenderContext_iOS.cpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#if AX_OS_IOS
#if AX_USE_VULKAN

#include "axVkRenderContext_iOS.h"

@interface axVkRenderContext_iOS_GLView : UIView {
	axVkRenderContext_iOS* _cpp;
}
@end

@implementation axVkRenderContext_iOS_GLView
+ (Class)layerClass {
	//this View has OpenGL layer
    return [CAEAGLLayer class];
}

-(instancetype) init:(axVkRenderContext_iOS*)cpp {
	self = [super init];
	_cpp = cpp;
	return self;
}
@end

axVkRenderContext_iOS::axVkRenderContext_iOS(axVkRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	ax_Vulkan_TODO
}

axVkRenderContext_iOS::~axVkRenderContext_iOS() {
}

void axVkRenderContext_iOS::_createBuffers() {
	ax_Vulkan_TODO
}

void axVkRenderContext_iOS::_destroyBuffers() {
	ax_Vulkan_TODO
}

void axVkRenderContext_iOS::onSetNeedToRender() {
	ax_Vulkan_TODO
}

void axVkRenderContext_iOS::setNativeViewRect(const axRect2f &rect) {
	ax_Vulkan_TODO
}

#endif //AX_USE_VULKAN
#endif //AX_OS_IOS
