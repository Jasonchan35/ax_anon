#pragma once

#if AX_USE_VULKAN

#if AX_USE_Win32Api
	#include "axVkRenderContext_Win32.h"
	using axVkRenderContext_Impl = axVkRenderContext_Win32;

#elif AX_OS_OSX
	#include "axVkRenderContext_MacOSX.h"
	using axVkRenderContext_Impl = axVkRenderContext_MacOSX;

#elif AX_OS_IOS
	#include "axVkRenderContext_iOS.h"
	using axVkRenderContext_Impl = axVkRenderContext_iOS;

#elif AX_OS_ANDROID
	#include "axVkRenderContext_Android.h"
	using axVkRenderContext_Impl = axVkRenderContext_Android;

#elif AX_USE_Gtk
	#include "axVkRenderContext_Gtk.h"
	using axVkRenderContext_Impl = axVkRenderContext_Gtk;

#elif AX_USE_X11
	#include "axVkRenderContext_X11.h"
	using axVkRenderContext_Impl = axVkRenderContext_X11;

#else
	#error

#endif

class axVkRenderContext : public axVkRenderContext_Impl {
	AX_RTTI_CLASS(axVkRenderContext, axVkRenderContext_Impl);
public:
	axVkRenderContext(axVkRenderer* renderer, CreateDesc& desc);
};

#endif //AX_USE_VULKAN
