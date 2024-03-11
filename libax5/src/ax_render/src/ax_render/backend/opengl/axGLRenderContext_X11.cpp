//
//  axGLRenderContext_X11.cpp
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axGLRenderContext.h"
#include "axGLCommonBase.h"

#if AX_USE_X11
#if AX_USE_OPENGL

//ref: https://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib

// The key problem is this: the function you use to get GLX extensions is, itself, an OpenGL extension
class axGLRenderContext_X11::FalseContext : public axNonCopyable {
public:
	FalseContext() {
	}

	~FalseContext() {
		if (_ctx) { glXDestroyContext(_dpy, _ctx); _ctx = nullptr; }
	}

	void create(Display* dpy) {
		_dpy = dpy;

		GLint attrib[] = {
			GLX_DOUBLEBUFFER,
			GLX_RGBA,
			GLX_DEPTH_SIZE,		24,
			GLX_STENCIL_SIZE, 	0, 
			0 //- End -
		};

		auto* vi = glXChooseVisual(_dpy, 0, attrib);
		if (!vi) throw axError_Undefined(AX_LOC);

		auto win = RootWindow(_dpy, vi->screen);
//		auto colormap = XCreateColormap(_dpy, win, vi->visual, ax_X11_AllocNone);

		_ctx = glXCreateContext(dpy, vi, NULL, GL_TRUE);
 		glXMakeCurrent(dpy, win, _ctx);

		glxewInit();
		glewInit();
		Util::checkGLError(AX_LOC);		 
	}

private:
	GLXContext _ctx = nullptr;
	Display* _dpy = nullptr;
};


//--------------------------------------------------

axGLRenderContext_X11::axGLRenderContext_X11(axGLRenderer* renderer, CreateDesc& desc)
	: Base(renderer, desc)
	, _x11_eventHandler(this)
{
	auto* app = axNativeUIApp::s_instance();
	_dpy = app->xdisplay();

	FalseContext falseContext;

	GLXContext sharedContext = nullptr;

	axGLRenderer::ScopedRootContext scopedRootContext;

	auto* rootCtx = scopedRootContext.rootContext();
	if (rootCtx) sharedContext = rootCtx->_glcontext;

	if (!sharedContext) {
		// create False context in order to init glew to have wglCreateContextAttribsARB()
		falseContext.create(_dpy); // require at least one per thread
	}
	
	static int visualAttrs[] = {
		GLX_X_RENDERABLE    , ax_X11_True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , ax_X11_True,
		
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		0
	};

	// Pick the FB config/visual with the most samples per pixel
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int fbcount = 0;

	auto scr = DefaultScreen(_dpy);
	GLXFBConfig* fbc = glXChooseFBConfig(_dpy, scr, visualAttrs, &fbcount);

	int i;
	for (i=0; i<fbcount; ++i) {
		XVisualInfo *vi = glXGetVisualFromFBConfig( _dpy, fbc[i] );
		if ( vi ) {
			int samp_buf, samples;
			glXGetFBConfigAttrib( _dpy, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( _dpy, fbc[i], GLX_SAMPLES       , &samples  );

			// AX_LOG("Matching fbconfig {?}, visual ID {?}: SAMPLE_BUFFERS = {?}, SAMPLES = {?}\n", 
			// 		i, vi -> visualid, samp_buf, samples );

			if ((best_fbc < 0 || samp_buf) && samples > best_num_samp) {
				best_fbc = i, best_num_samp = samples;
			}
			if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
				worst_fbc = i, worst_num_samp = samples;
			}
		}
		XFree( vi );
	}

	if (desc.window) {
		auto parentWin = desc.window->xwindow();
		if (!parentWin) throw axError_Undefined(AX_LOC);

		XVisualInfo* vi = glXGetVisualFromFBConfig( _dpy, fbc[best_fbc] );
		auto colormap = XCreateColormap(_dpy, parentWin, vi->visual, ax_X11_AllocNone);

		unsigned long swaMask = 0;
		XSetWindowAttributes swa;

		//swa.background_pixel = XWhitePixel(_dpy, vi->screen);
		//swaMask |= CWBackPixel;

		swa.colormap = colormap;
		swaMask |= ax_X11_CWColormap;
		
		const int border_width = 1;
		#if 1
			_win = XCreateWindow(	_dpy, parentWin, 
									0, 0, 16, 16,
									border_width, 
									vi->depth, InputOutput, vi->visual, swaMask, &swa);
		#else
			AX_UNUSED(swa);
			_win = XCreateSimpleWindow(_dpy, parentWin, 0,0, 16, 16, border_width, BlackPixel(_dpy, scr), WhitePixel(_dpy, scr));
		#endif

		//AX_LOG("created GL RenderContext window {?} parent={?}", _win, parentWin);

		XStoreName(_dpy, _win, "axGLRenderContext_X11");
		_x11_eventHandler.create(_dpy, _win);

		XMapWindow(_dpy, _win);
	}

	int glx_major, glx_minor;
	if ( !glXQueryVersion(_dpy, &glx_major, &glx_minor)) {
		throw axError_Undefined(AX_LOC);
	}
	
	if ((glx_major == 1 && glx_minor < 3) || glx_major < 1) {
		throw axError_Undefined(AX_LOC);
	}

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	//auto ver = axGLUtil::getGLString(GL_VERSION);
	//AX_LOG("GL version {?}", axStrView_c_str(ver));

	//axGLUtil::logExtensions();	

	if (major < 3) {
		AX_LOG("OpenGL 3.3 is required");
		throw axError_Undefined(AX_LOC);
	}

	int contextAttrs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		//GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	_glcontext = glXCreateContextAttribsARB(_dpy, fbc[best_fbc], sharedContext, true, contextAttrs);
	if (!_glcontext) {
		AX_LOG("OpenGL 3.3 is required");
		throw axError_Undefined(AX_LOC);
	}
//	AX_LOG("create GLX Context {?} {?} {?}", (void*)this, (void*)ctx, win);
}

void axGLRenderContext_X11::onX11_Expose() {
	if (_eventHandler) {
		makeGLContextCurrent();
		_eventHandler->render(this);
	}
}

void axGLRenderContext_X11::onX11_Destroy() {
	ax_log_func_name();
}

void axGLRenderContext_X11::onX11_DidDestroy() {
	ax_log_func_name();
}

axGLRenderContext_X11::~axGLRenderContext_X11() {
	onPreDestroy();
	if (_win) {
		auto* app = axNativeUIApp::s_instance();
		XDeleteContext(_dpy, _win, app->xcontext()); // avoid callback before destroy glcontext
	}

	if (_glcontext) {
		glXDestroyContext(_dpy, _glcontext);
		_glcontext = nullptr;
	}
	if (_win) {
		XDestroyWindow(_dpy, _win);
		_win = 0;
	}
}

void axGLRenderContext_X11::cmdSwapBuffers() {
	AX_ASSERT(onIsGLContextCurrent());
	auto vsync = _renderer->vsync();
	if (_lastVSync != vsync) {
		if (glXSwapIntervalEXT) {
			glXSwapIntervalEXT(_dpy, _win, vsync ? 1 : 0);
		}
		_lastVSync = vsync;
	}

	glXSwapBuffers(_dpy, _win);
}

void axGLRenderContext_X11::onMakeGLContextCurrent() {
	if (!glXMakeCurrent(_dpy, _win, _glcontext)) {
		Util::checkGLError(AX_LOC);
	}
}

void axGLRenderContext_X11::onUnmakeGLContextCurrent() {
	glXMakeCurrent(nullptr, 0, nullptr);
}


bool axGLRenderContext_X11::onIsGLContextCurrent() {
	if (!_glcontext) return false;
	return _glcontext == glXGetCurrentContext(); 	
}
	
void axGLRenderContext_X11::onSetNativeViewRect(const axRect2f& rect) {
	//X Window does not accept size < 1
	unsigned int w = ax_max(1U, (unsigned int)rect.w);
	unsigned int h = ax_max(1U, (unsigned int)rect.h);

	XMoveWindow(  _dpy, _win, (int)rect.x, (int)rect.y);
	XResizeWindow(_dpy, _win, w, h);
}

void axGLRenderContext_X11::onSetNeedToRender() {
	auto* app = axNativeUIApp_X11::s_instance();
	app->_addHandlerNeedExpose(_win, _x11_eventHandler);
}

#endif //AX_USE_OPENGL
#endif //AX_USE_X11
