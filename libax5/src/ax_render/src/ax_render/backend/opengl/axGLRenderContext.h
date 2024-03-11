#pragma once

#if AX_USE_OPENGL

#if AX_USE_Win32Api
	#include "axGLRenderContext_Win32.h"
	using axGLRenderContext_Impl = axGLRenderContext_Win32;

#elif AX_OS_OSX
	#include "axGLRenderContext_MacOSX.h"
	using axGLRenderContext_Impl = axGLRenderContext_MacOSX;

#elif AX_OS_IOS
	#include "axGLRenderContext_iOS.h"
	using axGLRenderContext_Impl = axGLRenderContext_iOS;

#elif AX_OS_ANDROID
	#include "axGLRenderContext_Android.h"
	using axGLRenderContext_Impl = axGLRenderContext_Android;

#elif AX_USE_Gtk
	#include "axGLRenderContext_Gtk.h"
	using axGLRenderContext_Impl = axGLRenderContext_Gtk;

#elif AX_USE_X11
	#include "axGLRenderContext_X11.h"
	using axGLRenderContext_Impl = axGLRenderContext_X11;

#else
	#error

#endif

class axGLRenderContext : public axGLRenderContext_Impl {
	AX_RTTI_CLASS(axGLRenderContext, axGLRenderContext_Impl);
public:
	axGLRenderContext(axGLRenderer* renderer, CreateDesc& desc);
};

#endif //AX_USE_OPENGL
