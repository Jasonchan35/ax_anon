//
//  axNativeUIEvent_MacOSX.h
//  libax_core
//
//  Created by Jason on 2018-06-09.
//
//

#pragma once

#if AX_OS_MACOSX

#include "../axNativeUIEvent.h"

enum class axNativeUI_CustomNSEventSubType : axInt16 {
	Unknown,
	CustomAppEvent,
	CustomDestroyWindow,
};

AX_STATIC_ASSERT(sizeof(axNativeUI_CustomNSEventSubType) == sizeof(NSEventSubtype));

class axNativeUIEventUtil_MacOSX {
public:
	static axNativeUIKeyCode		convertKey(NSEvent* inEvent);
	static axNativeUIEventModifier	convertModifierFlags(NSEventModifierFlags mf);
};

#endif
