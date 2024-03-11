#include <ax_render/axRenderer.h>
#include <ax_render/axRenderContext.h>
#include <ax_render/axRenderRequest.h>
#include <ax_render/axRenderBuiltInAssets.h>
#include "shader/axShader.h"

#if AX_USE_OPENGL
	#include <ax_render/backend/opengl/axGLRenderer.h>
#endif

#if AX_USE_VULKAN
	#include <ax_render/backend/vulkan/axVkRenderer.h>
#endif

#if AX_USE_DX11
	#include <ax_render/backend/dx11/axDX11Renderer.h>
#endif

#if AX_USE_DX12
	#include <ax_render/backend/dx12/axDX12Renderer.h>
#endif

#if AX_USE_METAL
	#include <ax_render/backend/metal/axMTLRenderer.h>
#endif


axRenderer* axRenderer_instance;

axRenderer* axRenderer::s_instance() {
	return axRenderer_instance;
}

axSPtr<axRenderer> axRenderer_create(axRenderer_CreateDesc& desc) {
	axSPtr<axRenderer> o;
	switch (desc.api) {
		#if AX_USE_OPENGL
			case axRenderApi::OpenGL:	o = ax_new axGLRenderer(desc); break;
		#endif
		#if AX_USE_VULKAN
			case axRenderApi::Vulkan:	o = ax_new axVkRenderer(desc); break;
		#endif
		#if AX_USE_DX11
			case axRenderApi::DX11:		o = ax_new axDX11Renderer(desc); break;
		#endif
		#if AX_USE_DX12
			case axRenderApi::DX12:		o = ax_new axDX12Renderer(desc); break;
		#endif
		#if AX_USE_METAL
			case axRenderApi::Metal:	o = ax_new(axMTLRenderer(desc)); break;
		#endif
		default:	throw axError_Undefined(AX_LOC); break;
	}

	o->onCreate();
	return o;
}

axRenderer_CreateDesc::axRenderer_CreateDesc() {
	#if AX_OS_WINDOWS
		api = axRenderApi::DX11;
	#elif AX_OS_OSX || AX_OS_IOS
		api = axRenderApi::Metal;
	#elif AX_OS_LINUX
		api = axRenderApi::OpenGL;
	#endif

	multithread = false;
}

axRenderer::axRenderer(CreateDesc& desc) 
	: _api(desc.api)
	, _multithread(desc.multithread)
	, _vsync(true)
{
	AX_ASSERT(axRenderer_instance == nullptr);
	axRenderer_instance = this;

	if (desc.multithread) {
		_renderThread.start("axRenderer", [this]() {
			onRenderThread();
		});

	// wait started
		auto data = _mdata.scopedLock();
		while (!data->started) {
			data.wait();
		}
	}
	_builtInAssets = ax_new BuiltInAssets();
}

void axRenderer::onCreate() {
	for (axInt i = 0; i < axRender_kMaxFrameAheadCount; i++)
		_freeRequests.append(onCreateRenderRequest());
	_pendingRequests.setMaxSize(1);
}

axRenderer::~axRenderer() {
	onDestroy();
	axRenderer_instance = nullptr;
}

axSPtr<axRenderMultiGpuBuffer> axRenderer::createMultiGpuRenderBuffer(axRenderGpuBuffer_CreateDesc& desc) {
	auto p = axSPtr_new(axRenderMultiGpuBuffer);
	p->create(desc);
	return p;
}

axSPtr<axShader> axRenderer::createShaderFromFile(axStrView filename) {
	auto s = onCreateShader();
	s->loadFile(filename);
	return s;
}

axSPtr<axTexture2D> axRenderer::createTexture2D(const axImage& image, const SamplerState& samplerState) {
	axTexture2D_CreateDesc desc;
	desc.colorType		= image.colorType();
	desc.size			= image.size();
	desc.samplerState	= samplerState;

	axTexture2D_UploadRequest upload;
	upload.imageInfo = image.info();
	upload.pixelData = image.pixelData();
	desc.uploadRequest = &upload;

	return createTexture2D(desc);
}

axSPtr<axTexture2D> axRenderer::createTexture2DFromFile(axStrView filename, const SamplerState& samplerState) {
	axImage image;
	image.loadFile(filename);
	image.convertToPremultipiedAlpha();
	return createTexture2D(image, samplerState);
}

void axRenderer::onDestroy() {
	{
		while (!_checkCompletedRequest()) {
			ax_sleep(5); // wait GPU to complete all request
		}

		_pendingRequests.clear();
		_freeRequests.clear();
	}

	if (_builtInAssets) {
		ax_delete(_builtInAssets);
		_builtInAssets = nullptr;
	}
}

bool axRenderer::_checkCompletedRequest() {
	while (auto req = _processingRequests.popHead()) {
		if (!req->completedFence()->isDone()) {
			_processingRequests.insert(ax_move(req));
			return false;
		}

		req->gpuEnded();
		_freeRequests.append(ax_move(req));
	}

	return true; // no more request in processingRequests
}

axUPtr<axRenderRequest> axRenderer::newRequest(axRenderContext* renderContext, const axRect2f& worldRect) {
	axUPtr<axRenderRequest> req;

	for (;;) {
		req = _freeRequests.timedWaitHead(5);
		if (req) break;

		_checkCompletedRequest();
	}

	auto seqId = getNextRenderSeqId();
	req->reset(seqId, renderContext, worldRect);
	return req;
}

void axRenderer::freeRequest(axUPtr<axRenderRequest> && req) {
	_freeRequests.append(ax_move(req));
}

void axRenderer::submitRequest(axUPtr<axRenderRequest> && req) {
	AX_ZoneScoped;

	req->updateGpuResource();
	_pendingRequests.append(ax_move(req));

	if (!_multithread) {
		onGpuRenderJob();
	}
}

bool axRenderer::onGpuRenderJob() {
	AX_ZoneScoped;

	auto req = _pendingRequests.popHead();
	if (!req) return false;

	req->_swapBuffers();

	req->renderContext()->onDispatchCommands(*req);
	_processingRequests.append(ax_move(req));

	return true;
}

void axRenderer::onRenderThread() {
	onRenderThreadStart();
	{
		auto data = _mdata.scopedLock();
		data->started = true;
	}

	for (;;) {
		AX_ZoneScoped;

		_checkCompletedRequest();

		{
			auto data = _mdata.scopedLock();
			if (data->quit) break;
		}

		if (!onGpuRenderJob()) {
			auto data = _mdata.scopedLock();
			data.timedWait(5);
		}
	}
}