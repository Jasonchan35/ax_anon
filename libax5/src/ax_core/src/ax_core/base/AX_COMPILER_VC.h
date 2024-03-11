#pragma once

#if AX_COMPILER_VC

// #pragma warning( disable: 4100 )// unreferenced formal parameter <- unused function parameter
#define _CRT_SECURE_NO_WARNINGS		1

//enable warning
//#pragma warning(1:4668) //Enable warning for directive is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(1:4263) // 'function': member function does not override any base class virtual member function
#pragma warning(1:4266) // 'function': no override available for virtual member function from base 'type'; function is hidden

//disable warning
#pragma warning(disable:4595) // non-member operator new or delete functions may not be declared inline
#pragma warning(disable:4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4702) // unreachable code

//#pragma warning(disable:4189) // local variable is initialized but not referenced
//#pragma warning(disable:4100) // unreferenced formal parameter

#include <cstdint>
#include <crtdbg.h>
#include <assert.h>
#include <conio.h>

#define ax_likely(x)       (x)
#define ax_unlikely(x)     (x)

#define ax_debug_break()	_CrtDbgBreak()

#if AX_GEN_CONFIG_Debug
	inline void ax_assert_platform_impl(const char* msg, const char* filename, int lineNumber) {
		if (1 == _CrtDbgReport(_CRT_ASSERT, filename, lineNumber, "libax", "%s", msg)) {
			_CrtDbgBreak();
		}
	}
#else
	inline void ax_assert_platform_impl(const char* /* msg */, const char* /* filename */, int /* lineNumber */) {
		assert(false);
	}
#endif

#endif // #if AX_COMPILER_VC
