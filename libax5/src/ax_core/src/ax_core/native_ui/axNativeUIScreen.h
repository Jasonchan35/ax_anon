//
//  axNativeUIScreen.hpp
//  libax_core
//
//  Created by Jason on 9/5/2020.
//

#pragma once

class axNativeUIScreen { // copyable
public:

#if AX_USE_Win32Api
	using Handle = HMONITOR;
	const Handle kInvalid = NULL;

#elif AX_OS_MACOSX
	using Handle = NSScreen*;
	const Handle kInvalid = nil;
	
//#elif AX_OS_IOS
//
//#elif AX_OS_ANDROID
//
//#elif AX_USE_Gtk
//
#elif AX_USE_X11
	using Handle = Screen*;
	const Handle kInvalid = nullptr;

#else
	using Handle = axInt;
	const Handle kInvalid = 0;

#endif

	axNativeUIScreen() = default;
	axNativeUIScreen(Handle h) : _h(h) {}

	Handle	handle() { return _h; }

	axRect2f	worldRect();

private:
	Handle	_h = kInvalid;
};
