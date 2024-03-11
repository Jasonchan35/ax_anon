#include <ax_core/system/axSystemInfo.h>
#include "axNativeUIApp_Base.h"

axNativeUIApp_Base::axNativeUIApp_Base()
	: Base()
	, _pendingCustomAppEvent(false)
{
}

void axNativeUIApp_Base::requestCustomAppEvent() {
	if (_pendingCustomAppEvent) return;
	if (!onRequestNativeCustomAppEvent()) return;
	_pendingCustomAppEvent = true;
}

void axNativeUIApp_Base::handleCustomAppEvent(bool force) {
	if (_pendingCustomAppEvent || force) {
		_pendingCustomAppEvent = false;
		onHandleCustomAppEvent();
	}
}
