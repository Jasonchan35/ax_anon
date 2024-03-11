#if AX_OS_ANDROID
#if AX_USE_OPENGL

#include "axGLRenderContext_Android.h"

axGLRenderContext_Android::axGLRenderContext_Android(axGLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
}

axGLRenderContext_Android::~axGLRenderContext_Android() {
}

void axGLRenderContext_Android::onMakeGLContextCurrent() {
}

void axGLRenderContext_Android::onUnmakeGLContextCurrent() {
}

bool axGLRenderContext_Android::onIsGLContextCurrent() {
	return false;
}

void axGLRenderContext_Android::cmdSwapBuffers() {
}

void axGLRenderContext_Android::onSetNeedToRender() {
}

void axGLRenderContext_Android::setNativeViewRect(const axRect2f &rect) {
}

#endif //AX_USE_OPENGL
#endif //AX_OS_ANDROID
