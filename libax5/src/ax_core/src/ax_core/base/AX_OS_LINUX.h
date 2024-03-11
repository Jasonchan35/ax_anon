#pragma once

#if AX_OS_LINUX

#if AX_OS_ANDROID
    #include "ax_os_android.h"

#else
	#define AX_USE_X11 1
	// #define AX_USE_Gtk 1

#endif

#include <sys/mman.h>

#endif // #if AX_OS_LINUX