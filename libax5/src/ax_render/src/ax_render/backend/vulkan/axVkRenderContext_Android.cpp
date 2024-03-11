#if AX_OS_ANDROID
#if AX_USE_VULKAN

#include "axVkRenderContext_Android.h"

axVkRenderContext_Android::axVkRenderContext_Android(axVkRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
}

axVkRenderContext_Android::~axVkRenderContext_Android() {
}

void axVkRenderContext_Android::onMakeCurrent() {
}

bool axVkRenderContext_Android::onIsCurrent() {
	return false;
}

void axVkRenderContext_Android::cmdSwapBuffers() {
}

void axVkRenderContext_Android::onSetNeedToRender() {
}

void axVkRenderContext_Android::setNativeViewRect(const axRect2f &rect) {
}

#endif //AX_USE_VULKAN
#endif //AX_OS_ANDROID
