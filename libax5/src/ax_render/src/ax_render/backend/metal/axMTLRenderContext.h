#pragma once

#if AX_USE_METAL

#if AX_USE_Win32Api
	#include "axMTLRenderContext_Win32.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_Win32;

#elif AX_OS_OSX
	#include "axMTLRenderContext_MacOSX.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_MacOSX;

#elif AX_OS_IOS
	#include "axMTLRenderContext_iOS.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_iOS;

#elif AX_OS_ANDROID
	#include "axMTLRenderContext_Android.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_Android;

#elif AX_USE_Gtk
	#include "axMTLRenderContext_Gtk.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_Gtk;

#elif AX_USE_X11
	#include "axMTLRenderContext_X11.h"
	using axMTLRenderContext_Impl = axMTLRenderContext_X11;

#else
	#error

#endif

class axMTLRenderContext : public axMTLRenderContext_Impl {
	AX_RTTI_CLASS(axMTLRenderContext, axMTLRenderContext_Impl);
public:
	axMTLRenderContext(axMTLRenderer* renderer, CreateDesc& desc);
};

#endif //AX_USE_METAL
