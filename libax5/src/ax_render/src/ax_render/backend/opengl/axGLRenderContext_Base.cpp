#if AX_USE_OPENGL

#include "axGLRenderer.h"
#include "axGLRenderContext_Base.h"
#include "axGLCommonBase.h"
#include "axGLCommandDispatcher.h"

thread_local axGLRenderContext_Base* axGLRenderContext_Base::ts_threadCurrent = nullptr;


axGLRenderContext_Base::ScopedContext::ScopedContext(axGLRenderContext_Base* ctx) {
	_ctx = ctx;
	_prev = axGLRenderContext_Base::s_makeGLContextCurrent(ctx);
}

axGLRenderContext_Base::ScopedContext::~ScopedContext() {
	axGLRenderContext_Base::s_makeGLContextCurrent(_prev);
}


axGLRenderContext_Base::axGLRenderContext_Base(axGLRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
}

void axGLRenderContext_Base::onPreDestroy() {
	auto* prev = s_makeGLContextCurrent(this);
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}
	unmakeGLContextCurrent();

	if (prev != this) {
		s_makeGLContextCurrent(prev);
	}
}

void axGLRenderContext_Base::onDispatchCommands(axRenderRequest& req) {
	ScopedContext scopedContext(this);

	AX_TracyGpuZone("GL");


	if (!_vao) {
		glGenVertexArrays(1, &_vao);
		if (!_vao) {
			Util::checkGLError(AX_LOC);
			throw axError_Undefined(AX_LOC);
		}
	}	
 	glBindVertexArray(_vao);
	Util::checkGLError(AX_LOC);

//---------------
	axGLCommandDispatcher dispatcher(this, &req);
	dispatcher.dispatch();

	glFlush(); // force execute, otherwise glFenceSync() does not signal on MacOSX, and this is non-blocking (unlike glFinish)
}

axGLRenderer* axGLRenderContext_Base::renderer() {
	return static_cast<axGLRenderer*>(Base::renderer());
}

void axGLRenderContext_Base::getGLInfo(axString& outInfo) {
	outInfo.format("Vender={?}\n"
		"Renderer={?}\n"
		"Version={?}\n",
		axGLUtil::getGLString(GL_VENDOR),
		axGLUtil::getGLString(GL_RENDERER),
		axGLUtil::getGLString(GL_VERSION));
}

axGLRenderContext_Base* axGLRenderContext_Base::s_makeGLContextCurrent(axGLRenderContext_Base* ctx) {
	auto* prev = ts_threadCurrent;
	if (ctx == prev) {
#if _DEBUG
		if (ctx) {
			if (!ctx->onIsGLContextCurrent()) {
				throw axError_Undefined(AX_LOC);
			}

			if (!ctx->_currentThread.isCurrentThread()) {
				throw axError_Undefined(AX_LOC);
			}
		}
#endif
		return prev;
	}

	if (prev) {
		prev->unmakeGLContextCurrent(); // remove it from current thread, before another thread makeCurrent()
	}

	if (ctx) {
		if (ctx->_currentThread) {
			throw axError_Undefined(AX_LOC);
		}
		ctx->onMakeGLContextCurrent();

		ctx->_currentThread.setToCurrentThread();
		ts_threadCurrent = ctx;
	} else {
		ts_threadCurrent = nullptr;
	}

	return prev;
}

void axGLRenderContext_Base::unmakeGLContextCurrent() {
	if (!isGLContextCurrent()) {
	// TODO
	//	AX_ASSERT(isGLContextCurrent());
	}

	ts_threadCurrent = nullptr;
	this->_currentThread.reset();
	onUnmakeGLContextCurrent();
}

#endif //AX_USE_OPENGL
