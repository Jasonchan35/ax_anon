#pragma once

#if AX_USE_OPENGL

#include "../../axRenderer.h"
#include "../../vertex/axRenderVertexDesc.h"
#include "axGLRenderContext.h"

class axGLRenderer : public axRenderer {
	AX_RTTI_CLASS(axGLRenderer, axRenderer)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using Util = axGLUtil;

	axGLRenderer(CreateDesc& desc);
	virtual ~axGLRenderer();

	virtual void onRenderThreadStart() override;

	axRenderer_InterfaceFunctions(axGL);

	axSPtr<axGLRenderContext>	createRenderContext(axRenderContext_CreateDesc& desc);

	struct RootContext {
		axSPtr<axGLRenderContext>	ctx;
	};

	using MutexProtectedRootContext = axMutexProtected<RootContext>;
	using RootContextScopedLock = MutexProtectedRootContext::ScopedLock;

	class ScopedRootContext : public axNonCopyable {
	public:
		ScopedRootContext();
		axGLRenderContext* rootContext() { return _rootContext; }

	private:
		RootContextScopedLock _lock;
		axGLRenderContext* _rootContext = nullptr;
		axGLRenderContext::ScopedContext	_scopedContext;
	};

	struct GLVersion {
		GLint major = 0;
		GLint minor = 0;

		bool require(GLint major_, GLint minor_) const {
			return major >= major_ && minor >= minor_;
		}
	};

	const GLVersion	glVersion() const { return _glVersion; }

protected:
	virtual void onGetMemoryInfo(MemoryInfo& info) override;

private:
	GLVersion	_glVersion;
	axArray<axStrView>	_glExtensionList;

	MutexProtectedRootContext	_rootContext;
	axSPtr<axGLRenderContext>	_mainThreadContext;

	struct MD {
		axArray< axSPtr<axGLRenderContext>, 32 > jobThreadContexts; // BUG !!!   axSPtr move crash !!!!!!!!!!!!!
	};

	axMutexProtected<MD> _md;
};

#endif ///AX_USE_OPENGL
