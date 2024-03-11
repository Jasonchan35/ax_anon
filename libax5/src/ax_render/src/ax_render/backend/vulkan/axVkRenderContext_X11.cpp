//
//  axVkRenderContext_X11.cpp
//  libax_core
//
//  Created by Jason on 2020-04-25
//
//

#include "axVkRenderContext.h"
#include "axVkCommonBase.h"
#include "axVkRenderer.h"

#if AX_USE_X11
#if AX_USE_VULKAN

axVkRenderContext_X11::axVkRenderContext_X11(axVkRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
	, _x11_eventHandler(this)
{
	auto* app = axNativeUIApp::s_instance();
	_dpy = app->xdisplay();

	if (desc.window) {
		auto parentWin = desc.window->xwindow();
		auto screen	= XDefaultScreen(_dpy);
		auto depth	= DefaultDepth(_dpy, screen);
		auto visual	= DefaultVisual(_dpy, screen);

		unsigned long swaMask = 0;
		XSetWindowAttributes swa = {};

		swa.colormap = XCreateColormap(_dpy, parentWin, visual, ax_X11_AllocNone);
		swaMask |= ax_X11_CWColormap;

		const int border_width = 1;
		_win = XCreateWindow(	_dpy, parentWin, 
								0, 0, 16, 16,
								border_width, 
								depth, InputOutput, visual, swaMask, &swa);

		_x11_eventHandler.create(_dpy, _win);
		XMapWindow(_dpy, _win);
	}

	_createDevice();
}

void axVkRenderContext_X11::onCreateVkSurface(axCppVkSurfaceKHR& surface) {
	auto& device = renderer()->device();
	surface.create_x11(*device.physicalDevice(), _dpy, _win);
}

void axVkRenderContext_X11::onX11_Expose() {
	if (_eventHandler) {
		_eventHandler->render(this);
	}
}

void axVkRenderContext_X11::onX11_Destroy() {
	ax_log_func_name();
}

void axVkRenderContext_X11::onX11_DidDestroy() {
	ax_log_func_name();
}

axVkRenderContext_X11::~axVkRenderContext_X11() {
	onPreDestroy();
}
	
void axVkRenderContext_X11::onSetNativeViewRect(const axRect2f& rect) {
	//X Window does not accept size < 1
	unsigned int w = ax_max(1U, (unsigned int)rect.w);
	unsigned int h = ax_max(1U, (unsigned int)rect.h);

	XMoveWindow(  _dpy, _win, (int)rect.x, (int)rect.y);
	XResizeWindow(_dpy, _win, w, h);
}

void axVkRenderContext_X11::onSetNeedToRender() {
	auto* app = axNativeUIApp_X11::s_instance();
	app->_addHandlerNeedExpose(_win, _x11_eventHandler);
}

#endif //AX_USE_VULKAN
#endif //AX_USE_X11
