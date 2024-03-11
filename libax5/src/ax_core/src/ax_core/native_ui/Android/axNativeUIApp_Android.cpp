#if AX_OS_ANDROID

#include "axNativeUIApp_Android.h"

axNativeUIApp_Android::~axNativeUIApp_Android() {
}

int	axNativeUIApp_Android::onRun() {
	AX_ASSERT_NOT_IMPLEMENTED();
	return -1;
}

void axNativeUIApp_Android::quit(int returnCode) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

axVec2f	axNativeUIApp_Android::s_getVirtualDesktopSize() {
	AX_ASSERT_NOT_IMPLEMENTED();
	return axVec2f(0);
}

#endif // AX_OS_ANDROID
