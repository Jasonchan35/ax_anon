#if AX_OS_ANDROID

#include "axNativeUIWindow_Android.h"

axNativeUIWindow_Android::axNativeUIWindow_Android(CreateDesc& desc)
	: Base(desc)
{
}

axNativeUIWindow_Android::~axNativeUIWindow_Android() {
}

void axNativeUIWindow_Android::onSetNativeActive(bool b) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::onSetNativeWindowTitle(axStrView title) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::onSetNativeWorldPos(const axVec2f& v) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::onSetNativeSize(const axVec2f& v) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::onSetNativeVisible(bool b) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::doCloseWindow() {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUIWindow_Android::doDestroyWindow() {
	AX_ASSERT_NOT_IMPLEMENTED();
}

#endif // AX_OS_ANDROID
