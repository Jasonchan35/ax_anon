//
//  platform.h
//  ax_core
//
//  Created by Jason on 2015-03-14.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

#include <ax_detect_platform.h>

#include "../ax_core_config.h"

//--------------------------------------------
class axType;

class axStaticClass {
private:
	axStaticClass() = delete; 
	virtual void _cannotCreateInstance_() = 0;
};

class axNonCopyable {
public:
	axNonCopyable()	= default;
	axNonCopyable(axNonCopyable && r) = default;

	axNonCopyable 	( const axNonCopyable &s ) = delete; //!< not allow by default
	void operator=	( const axNonCopyable &s ) = delete; //!< not allow by default
};

//--------- COMPILER ----------

#if AX_COMPILER_GCC || AX_COMPILER_CLANG
	#include "AX_COMPILER_GCC.h"
#endif

#if AX_COMPILER_VC
	#include "AX_COMPILER_VC.h"
#endif

//--------- OS -------------
#if AX_OS_ANDROID
	#include "AX_OS_ANDROID.h"
#elif AX_OS_LINUX
	#include "AX_OS_LINUX.h"
#elif AX_OS_IOS
	#include "AX_OS_IOS.h"
#elif AX_OS_MACOSX
	#include "AX_OS_MACOSX.h"
#elif AX_OS_FREEBSD
	#include "AX_OS_FREEBSD.h"
#elif AX_OS_CYGWIN
	#define	AX_USE_PTHREAD	1
	#include "AX_OS_CYGWIN.h"
#elif AX_OS_MINGW
	#define	AX_USE_PTHREAD	1
	#include "AX_OS_MINGW.h"
#endif

//------------------------
#if AX_OS_FREEBSD || AX_OS_LINUX || AX_OS_MACOSX || AX_OS_IOS || AX_OS_ANDROID
	#define	AX_OS_UNIX		1
	#define	AX_USE_PTHREAD	1
	#include "AX_OS_UNIX.h"

#elif AX_OS_WIN32 || AX_OS_WIN64 || AX_OS_WinCE
	#undef  AX_OS_WINDOWS
	#define AX_OS_WINDOWS	1
	#include "AX_OS_WINDOWS.h"
#endif

//--- C++ headers ------
#include <atomic>
#include <limits>
#include <cmath>
#include <exception>
#include <new>
#include <float.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <type_traits>
#include <wchar.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <tuple>
#include <vector>
#include <functional>
#include <chrono>

//#include <shared_mutex> //C++17

void ax_core_init();

//-------------------
#include "ax_macro.h"
#include "ax_basic_types.h"
#include "ax_safe_assign.h"
#include "ax_math.h"
#include "ax_utils.h"
#include "ax_bitwise.h"
#include "axAtomic.h"
#include "AX_OBJC.h"
#include "ax_debug_helper.h"
#include "axPushable.h"
#include "axByteOrder.h"

#if AX_USE_X11
	#include "AX_X11.h"
#endif

#if AX_USE_Gtk
	#include "AX_GTK.h"
#endif

#if TRACY_ENABLE
	#include <tracy/Tracy.hpp>
#endif