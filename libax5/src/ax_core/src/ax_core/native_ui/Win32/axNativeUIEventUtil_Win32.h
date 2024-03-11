#pragma once

#if AX_USE_Win32Api
#include "../axNativeUIEvent.h"

enum {
	ax_WM_USER_HasCustomAppEvent = WM_USER + 1,
};

class axNativeUIEventUtil_Win32 {
public:
	static axNativeUIKeyCode convertKey(WPARAM vkey);
};

#endif