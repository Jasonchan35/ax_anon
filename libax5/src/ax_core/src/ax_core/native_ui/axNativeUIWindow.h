#pragma once

#if AX_USE_Win32Api
	#include "Win32/axNativeUIWindow_Win32.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_Win32;

#elif AX_OS_MACOSX
	#include "MacOSX/axNativeUIWindow_MacOSX.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_MacOSX;

#elif AX_OS_IOS
	#include "iOS/axNativeUIWindow_iOS.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_iOS;

#elif AX_OS_ANDROID
	#include "Android/axNativeUIWindow_Android.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_Android;

#elif AX_USE_Gtk
	#include "Gtk/axNativeUIWindow_Gtk.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_Gtk;

#elif AX_USE_X11
	#include "X11/axNativeUIWindow_X11.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_X11;

#else
	#include "Null/axNativeUIWindow_Null.h"
	using axNativeUIWindow_Impl = axNativeUIWindow_Null;
#endif

class axNativeUIWindow : public axNativeUIWindow_Impl {
	AX_RTTI_CLASS(axNativeUIWindow, axNativeUIWindow_Impl)
public:
	axNativeUIWindow(CreateDesc& desc) : Base(desc) {}
};
