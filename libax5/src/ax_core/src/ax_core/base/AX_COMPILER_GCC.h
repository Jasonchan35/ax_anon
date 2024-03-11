#pragma once

#if AX_COMPILER_CLANG | AX_COMPILER_GCC

#include <assert.h>
#if AX_OS_ANDROID
	#include <android/log.h>
#else
	#include <iostream>
#endif

#define ax_likely(x)       __builtin_expect(!!(x),1)
#define ax_unlikely(x)     __builtin_expect(!!(x),0)

#define ax_debug_break()	assert(false)
//#define ax_debug_break()	_asm__("int $3\n" : : )

inline void ax_assert_platform_impl(const char* msg, const char* /* filename */, int /* lineNumber */) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", msg);
#else
    std::cerr << msg;
#endif
	ax_debug_break();
}

#endif // #if AX_COMPILER_CLANG | AX_COMPILER_GCC
