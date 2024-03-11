#pragma once

#include "axNativeUIScreen.h"

#include "Base/axNativeUIApp_Base.h"

#if AX_USE_Win32Api
	#include "Win32/axNativeUIApp_Win32.h"
	using axNativeUIApp_Impl = axNativeUIApp_Win32;

#elif AX_OS_MACOSX
	#include "MacOSX/axNativeUIApp_MacOSX.h"
	using axNativeUIApp_Impl = axNativeUIApp_MacOSX;

#elif AX_OS_IOS
	#include "iOS/axNativeUIApp_iOS.h"
	using axNativeUIApp_Impl = axNativeUIApp_iOS;

#elif AX_USE_Gtk
	#include "Gtk/axNativeUIApp_Gtk.h"
	using axNativeUIApp_Impl = axNativeUIApp_Gtk;

#elif AX_USE_X11
	#include "X11/axNativeUIApp_X11.h"
	using axNativeUIApp_Impl = axNativeUIApp_X11;

#else
	#include "Null/axNativeUIApp_Null.h"
	using axNativeUIApp_Impl = axNativeUIApp_Null;
#endif

class axNativeUIApp : public axNativeUIApp_Impl {
	AX_RTTI_CLASS(axNativeUIApp, axNativeUIApp_Impl);
public:
	ax_DOWNCAST_GET_INSTANCE()
};
