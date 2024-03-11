//
//  axNativeUIScreen.cpp
//  libax_core
//
//  Created by Jason on 9/5/2020.
//

#include "axNativeUIScreen.h"

#if AX_USE_Win32Api

axRect2f	axNativeUIScreen::worldRect() {
	MONITORINFO info = {};
	info.cbSize = sizeof(info);
	if (_h) {
		if (GetMonitorInfoA(_h, &info)) {
			return axRect2f(info.rcWork);
		}
	}
	return axRect2f(0,0,0,0);
}

#elif AX_OS_MACOSX

axRect2f	axNativeUIScreen::worldRect() {
	if (!_h) return axRect2f(0,0,0,0);
	auto rc = axRect2f([_h frame]);
	return rc;
}

#elif AX_USE_X11

axRect2f axNativeUIScreen::worldRect() {
	if (!_h) return axRect2f(0,0,0,0);
	return axRect2f(0, 0, _h->width, _h->height);
}

#else
	axRect2f axNativeUIScreen::worldRect() { return axRect2f(0,0,0,0); }

#endif