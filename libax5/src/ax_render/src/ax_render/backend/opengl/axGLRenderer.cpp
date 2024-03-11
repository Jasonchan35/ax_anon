#if AX_USE_OPENGL

#include "axGLRenderer.h"
#include "axGLRenderContext.h"
#include "axGLStorageBuffer.h"
#include "axGLStageBuffer.h"
#include "axGLShader.h"
#include "axGLMaterial.h"
#include "axGLMaterialPass.h"
#include "axGLTexture.h"
#include "axGLFence.h"
#include "axGLRenderRequest.h"

axRenderer_InterfaceFunctions_Impl(axGL);

axGLRenderer::axGLRenderer(CreateDesc& desc) 
	: Base(desc)
{
	axRenderContext_CreateDesc ctxDesc;
	ctxDesc.debugName = "root";
	{
		auto rootContext = _rootContext.scopedLock();
		rootContext->ctx = createRenderContext(ctxDesc);
	}

	ctxDesc.debugName = "mainThread";
	_mainThreadContext = createRenderContext(ctxDesc);
	_mainThreadContext->makeGLContextCurrent();

	glGetIntegerv(GL_MAJOR_VERSION, &_glVersion.major);
	glGetIntegerv(GL_MINOR_VERSION, &_glVersion.minor);

	axGLUtil::getExtensionList(_glExtensionList);
	axGLUtil::logExtensions();

#if 1
	_adapterInfo.adapterName.format("{?} - OpenGL{?}",
		axGLUtil::getGLString(GL_RENDERER),
		axGLUtil::getGLString(GL_VERSION));
#else
	_adapterInfo.adapterName.format("{?}\n{?}\n{?}", 
		axGLUtil::getGLString(GL_VENDOR),
		axGLUtil::getGLString(GL_RENDERER),
		axGLUtil::getGLString(GL_VERSION)
	);
#endif
	_adapterInfo.shaderFloat64 = _glExtensionList.contains("GL_ARB_gpu_shader_fp64");
	_adapterInfo.computeShader = _glVersion.require(4, 3);
}

axGLRenderer::~axGLRenderer() {
	onDestroy();
}

void axGLRenderer::onRenderThreadStart() {
	axRenderContext_CreateDesc ctxDesc;
	ctxDesc.debugName = "renderThread";
	auto ctx = createRenderContext(ctxDesc);

	{
		auto md = _md.scopedLock();
		md->jobThreadContexts.emplaceBack(ax_move(ctx));
	}

	ctx->makeGLContextCurrent();
}

axSPtr<axGLRenderContext> axGLRenderer::createRenderContext(axRenderContext_CreateDesc& desc) {
	auto p = Base::createRenderContext(desc);
	if (!p) return nullptr;
	return ax_type_cast<axGLRenderContext>(p.ptr());
}

void axGLRenderer::onGetMemoryInfo(MemoryInfo& info) {
	if (GLEW_NVX_gpu_memory_info) {
		GLint total = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);

		GLint avail = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &avail);

		info.budget = axInt64(total) * 1024;
		info.used   = info.budget - axInt64(avail) * 1024;
		return;
	}

#if AX_USE_Win32Api

	if (wglGetGPUIDsAMD) {
		UINT uNoOfGPUs = wglGetGPUIDsAMD(0, 0);

		axArray<UINT, 16> uGPUIDs;
		uGPUIDs.resize(uNoOfGPUs);

		wglGetGPUIDsAMD(uNoOfGPUs, uGPUIDs.data());

		GLuint uTotalMemoryInMB = 0;
		wglGetGPUInfoAMD(	uGPUIDs[0],
							WGL_GPU_RAM_AMD,
							GL_UNSIGNED_INT,
							sizeof( GLuint ),
							&uTotalMemoryInMB );

		GLint nCurAvailMemoryInKB = 0;
		glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI,
							   &nCurAvailMemoryInKB );

		info.budget = axInt64(uTotalMemoryInMB) * 1024 * 1024;
		info.used   = info.budget - axInt64(nCurAvailMemoryInKB) * 1024;
		return;
	}
#endif
}

axGLRenderer::ScopedRootContext::ScopedRootContext() 
	: _lock(s_instance()->_rootContext.scopedLock())
	, _rootContext(_lock->ctx)
	, _scopedContext(_rootContext)
{
}

#endif //AX_USE_OPENGL
