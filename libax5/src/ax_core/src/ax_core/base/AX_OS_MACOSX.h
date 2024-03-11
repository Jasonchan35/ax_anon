#pragma once

#if AX_OS_MACOSX

#if AX_TRY_USE_OPENGL
	#define ax_HAS_GLEW 1
	#define GLEW_STATIC 1
	#include "glew/glew.h"
#endif

#define AX_TYPE_HAS_LONG		1

//#include <dirent.h>
//#include <iostream>
#include <pthread.h>
#include <libc.h>
#include <os/lock.h>

#if AX_OBJC
	#import <Foundation/Foundation.h>
	#import <CoreServices/CoreServices.h>
	#import <Cocoa/Cocoa.h>
	#import <AppKit/AppKit.h>

	//#import <AVFoundation/AVFoundation.h>
	#import <AudioToolbox/AudioToolbox.h>
	#import <libkern/OSAtomic.h>

#else
	#error "only support objc++, please set GCC_INPUT_FILETYPE = sourcecode.cpp.objcpp in xcode"
#endif

//#import <sys/sysctl.h>

#endif // #if AX_OS_MACOSX