#pragma once

#if AX_USE_X11

#include "../axNativeUIEvent.h"

class ax_X11_Util {
public:
	static axNativeUIKeyCode convertKey(KeySym key);

	static bool checkError(const axSourceLoc& loc, int errroCode);
};

#endif