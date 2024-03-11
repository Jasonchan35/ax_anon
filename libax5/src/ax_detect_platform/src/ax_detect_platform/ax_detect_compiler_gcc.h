#pragma once

#if ! (AX_COMPILER_CLANG | AX_COMPILER_GCC)
	#error
#endif

#if __cplusplus >= 199711L
	#define AX_CPLUSPLUS_03	1
#endif

#if __cplusplus >= 201103L 
	#define AX_CPLUSPLUS_11	1
#endif

#if __cplusplus >= 201402L
	#define AX_CPLUSPLUS_14	1
#endif

#if __cplusplus >= 201703L
	#define AX_CPLUSPLUS_17	1
#endif

#if __cplusplus >= 202002L
	#define AX_CPLUSPLUS_20	1
#endif



#if AX_COMPILER_CLANG
	#define AX_TYPE_HAS_SIZE_T		0  //linux 
	#define AX_TYPE_HAS_SSIZE_T		0  //linux 

	#define AX_OPTIMIZE_OFF			_Pragma("clang optimize off")
#else
	#define AX_TYPE_HAS_SIZE_T		0  //linux 
	#define AX_TYPE_HAS_SSIZE_T		0  //linux 

	#define AX_OPTIMIZE_OFF			_Pragma("GCC optimize(\"O0\")")
#endif

#if __GNUC__ >= 4
	#define	AX_DLL_EXPORT			__attribute__ ((visibility ("default")))
	#define AX_DLL_IMPORT			// __attribute__ ((visibility ("hidden")))
#else
	#define	AX_DLL_EXPORT
	#define AX_DLL_IMPORT
#endif


// #define nullptr	NULL

#define	AX_FUNC_NAME_SZ				__FUNCTION__
#define AX_FUNC_FULLNAME_SZ			__PRETTY_FUNCTION__

#define AX_DEPRECATED( f )		f __attribute__( (deprecated) )

#define AX_COMPILER_VER __cplusplus

#if AX_CPLUSPLUS_17
	#define AX_FALLTHROUGH		// [[fallthrough]]
	#define AX_NODISCARD		[[nodiscard]]
#else
	#define AX_FALLTHROUGH
	#define AX_NODISCARD
#endif

//#define ax_ALIGN(N)				__attribute__((aligned(N)))
//#define ax_ALIGN(N)				alignas(N) //c++11

#if AX_GEN_CONFIG_Debug
	#define	AX_INLINE      inline
#else
	#define	AX_INLINE		inline //__attribute__( (always_inline) )
#endif

//#define AX_THREAD_LOCAL	__thread
#define AX_THREAD_LOCAL thread_local //C++11


#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__)
	#define AX_CPU_x86_64      1

#elif defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
	#define AX_CPU_x86         1

#elif defined(__POWERPC__)
	#define AX_CPU_PowerPC     1

#elif defined(__aarch64__)
	#define AX_CPU_ARM64 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define AX_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define AX_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif

#elif defined(__arm__)
	#define AX_CPU_ARM 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define AX_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define AX_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif
#endif

#if	defined(__ARM_NEON__) || defined(__ARM_NEON)
	#define AX_CPU_FEATURE_ARM_NEON
#endif

#if __SSE__
	#define AX_CPU_FEATURE_SSE1			1
#endif

#if __SSE2__
	#define AX_CPU_FEATURE_SSE2			1
	#include <xmmintrin.h>
#endif

#if __SSE3__
	#define AX_CPU_FEATURE_SSE3			1
#endif

#if __SSSE3__
	#define AX_CPU_FEATURE_SSSE3		1
#endif

#if __SSE4_1__
	#define AX_CPU_FEATURE_SSE4_1		1
#endif

#if __SSE4_2__
	#define AX_CPU_FEATURE_SSE4_2		1
#endif

//os

#if _WIN64
	#define AX_OS_WIN64     1
	#define AX_OS_WINDOWS	1

#elif _WIN32
	#define AX_OS_WIN32     1
	#define AX_OS_WINDOWS	1

#elif __ANDROID__
	#define AX_OS_ANDROID	1

#elif __linux
	#define AX_OS_LINUX        1

#elif __FreeBSD__
	#define AX_OS_FREEBSD		1

#elif __APPLE__ && __MACH__
	#include <TargetConditionals.h>
	#if (TARGET_OF_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
		#define AX_OS_IOS		1
	#else
		#define AX_OS_MACOSX	1
	#endif

#elif __sun
	#define AX_OS_SOLARIS		1

#elif __CYGWIN__
    #define AX_OS_CYGWIN        1

#elif __MINGW32__
	#define AX_OS_MINGW			1
#endif

