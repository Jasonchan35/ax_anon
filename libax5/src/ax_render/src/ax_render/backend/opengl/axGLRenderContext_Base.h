#pragma once

#if AX_USE_OPENGL

//
//  axGLRenderContext_Base.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "../../axRenderContext.h"
#include "axGLCommonBase.h"

class axGLError : public axError {};
class axGLError_Undefined : public axGLError {};
class axGLRenderContext;
class axGLRenderer;
class axGLShader;
class axGLShaderPass;
class axGLMaterial;
class axGLRenderVertexBuffer;
class axGLRenderIndexBuffer;

class axGLRenderContext_Base : public axRenderContext {
	AX_RTTI_CLASS(axGLRenderContext_Base, axRenderContext)
public:
	using Util = axGLUtil;

	class ScopedContext : public axNonCopyable {
	public:
		ScopedContext(axGLRenderContext_Base* ctx);
		~ScopedContext();

	private:
		axGLRenderContext_Base* _ctx = nullptr;
		axGLRenderContext_Base* _prev = nullptr;
	};

	axGLRenderContext_Base(axGLRenderer* renderer, CreateDesc& desc);

	axGLRenderer* renderer();

	void getGLInfo(axString& outInfo);

	virtual void cmdSwapBuffers() = 0;

	static axGLRenderContext_Base* s_makeGLContextCurrent(axGLRenderContext_Base* ctx);

	axGLRenderContext_Base* makeGLContextCurrent() { return s_makeGLContextCurrent(this); }
	void unmakeGLContextCurrent();

	bool isGLContextCurrent() { return onIsGLContextCurrent(); }

protected:
	virtual void onMakeGLContextCurrent() = 0;
	virtual void onUnmakeGLContextCurrent() = 0;

	virtual bool onIsGLContextCurrent() = 0;

	virtual void onDispatchCommands(axRenderRequest& req) override;
	virtual void onPreDestroy() override;

	axThreadId _currentThread;

	GLuint _vao = 0;

private:
	thread_local static axGLRenderContext_Base* ts_threadCurrent;
};

#endif //AX_USE_OPENGL
