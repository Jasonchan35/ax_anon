#pragma once

/*
libax_detect_platform:
	- headers only
	- detect compile / OS
	- may used by external lib, so please keep it simple
*/

#ifdef __OBJC__
	#define AX_OBJC		1
#else
	#define AX_OBJC		0
#endif

//=========== Detect COMPILER ===============
#if defined(__clang__) 
	#define AX_COMPILER_CLANG	1
	#include "ax_detect_platform/ax_detect_compiler_gcc.h"

#elif defined(__GNUC__)
	#define AX_COMPILER_GCC		1
	#include "ax_detect_platform/ax_detect_compiler_gcc.h"

#elif defined(_MSC_VER)
	#define AX_COMPILER_VC		1
	#include "ax_detect_platform/ax_detect_compiler_vc.h"

#endif

#if AX_COMPILER_VC + AX_COMPILER_GCC + AX_COMPILER_CLANG != 1
    #error "Compiler should be specified"
#endif

//======== Detect CPU =============

// check CPU define
#if AX_CPU_x86_64 + AX_CPU_x86 + AX_CPU_PowerPC + AX_CPU_ARM + AX_CPU_ARM64 != 1
	#error CPU should be specified
#endif

#if AX_CPU_x86_64
	#define AX_CPU_LP64				1
	#define AX_CPU_ENDIAN_LITTLE	1
	#define AX_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AX_CPU_x86
	#define AX_CPU_LP32				1
	#define AX_CPU_ENDIAN_LITTLE	1
	#define AX_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AX_CPU_PowerPC
	#define AX_CPU_LP32				1
	#define AX_CPU_ENDIAN_BIG		1
	#define AX_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AX_CPU_ARM64
	#define AX_CPU_LP64				1
	#define AX_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AX_CPU_ARM
	#define AX_CPU_LP32				1
	#define AX_CPU_MISALIGNED_MEMORY_ACCESSES	1
#endif

#if AX_CPU_LP32 + AX_CPU_LP64 != 1
	#error CPU bits should be specified
#endif

#if AX_CPU_LP32
	#define AX_SIZEOF_POINTER		4
#endif

#if AX_CPU_LP64
	#define AX_SIZEOF_POINTER		8
#endif

#ifndef AX_SIZEOF_POINTER
	#error
#endif

#if AX_CPU_ENDIAN_BIG + AX_CPU_ENDIAN_LITTLE != 1
	#error CPU endian should be specified
#endif

//======== Detect OS ===============

#if AX_OS_WIN32 + AX_OS_WIN64 + AX_OS_WinCE \
	+ AX_OS_FREEBSD + AX_OS_LINUX \
	+ AX_OS_ANDROID \
	+ AX_OS_MACOSX  + AX_OS_IOS \
	+ AX_OS_CYGWIN  + AX_OS_MINGW != 1
	#error OS should be specified
#endif



//====================================

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif
