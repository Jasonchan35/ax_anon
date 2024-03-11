#pragma once

#pragma warning(disable: 4668) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'

#if !AX_COMPILER_VC	
	#error
#endif

#if _MSVC_LANG >= 199711L
	#define AX_CPLUSPLUS_03	1
#endif

#if _MSVC_LANG >= 201103L
	#define AX_CPLUSPLUS_11	1
#endif

#if _MSVC_LANG >= 201402L
	#define AX_CPLUSPLUS_14	1
#endif

#if _MSVC_LANG >= 201703L
	#define AX_CPLUSPLUS_17	1
#endif

#if _MSVC_LANG >= 202002L
	#define AX_CPLUSPLUS_20	1
#endif



#define AX_TYPE_HAS_SIZE_T		0
#define AX_TYPE_HAS_SSIZE_T		0
#define AX_TYPE_HAS_LONGLONG	0

#define AX_DLL_EXPORT			_declspec(dllexport)
#define AX_DLL_IMPORT			_declspec(dllimport)

#define	AX_FUNC_NAME_SZ			__FUNCTION__
#define AX_FUNC_FULLNAME_SZ		__FUNCSIG__

#define AX_DEPRECATED			__declspec(deprecated)

#define AX_COMPILER_VER _MSVC_LANG

#if AX_CPLUSPLUS_17
	#define AX_FALLTHROUGH	//	[[fallthrough]]
	#define AX_NODISCARD		[[nodiscard]]
#else
	#define AX_FALLTHROUGH
	#define AX_NODISCARD
#endif

//#define ax_ALIGN(N)				__declspec(align(N)) 
//#define ax_ALIGN(N)				alignas(N) //c++11

#define AX_OPTIMIZE_OFF			__pragma(optimize("", off))

#if 0 // AX_GEN_CONFIG_Debug
	#define	AX_INLINE		inline
#else
	#define	AX_INLINE		__forceinline
#endif

#define AX_THREAD_LOCAL			__declspec( thread )
//#define AX_THREAD_LOCAL thread_local //C++11

#if _MSC_VER < 1600
	#define	nullptr	NULL
#endif

//cpu
#if _M_X64
	#define AX_CPU_x86_64	1
	#define AX_CPU_FEATURE_SSE1		1
	#define AX_CPU_FEATURE_SSE2		1
	#define AX_CPU_FEATURE_SSE3		1

#elif _M_PPC
	#define AX_CPU_PowerPC     1
#else
	#define AX_CPU_x86         1
#endif

//os

#if _WIN64
	#define AX_OS_WIN64     1
	#define AX_OS_WINDOWS	1
#elif _WIN32
	#define AX_OS_WIN32     1
	#define AX_OS_WINDOWS	1
#elif _WINCE
	#define AX_OS_WinCE     1
	#define AX_OS_WINDOWS	1
#endif
