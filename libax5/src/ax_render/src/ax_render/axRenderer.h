#pragma once

#include <ax_render/ax_render_config.h>
#include <ax_render/image/axImage.h>
#include <ax_render/vertex/axRenderVertexDesc.h>
#include <ax_render/buffer/axRenderStorageBuffer.h>
#include <ax_render/font/axFont.h>
#include <ax_render/texture/axSamplerState.h>
#include <ax_render/axRenderFence.h>

#define axRenderApi_EnumList(E) \
	E(Unknown,) \
	E(OpenGL,) \
	E(DX11,) \
	E(DX12,) \
	E(Vulkan,) \
	E(Metal,) \
//-----
ax_ENUM_CLASS(axRenderApi, axInt8);

class axRenderRayTracing : public axObject {
	AX_RTTI_CLASS(axRenderRayTracing, axObject);
public:

};

class axRenderer_CreateDesc {
public:
	axRenderer_CreateDesc();
	axRenderApi	api;
	bool multithread;
};

class axRenderer : public axObject, public axRenderCommonBase {
	AX_RTTI_CLASS(axRenderer, axObject)
public:
	using BuiltInAssets = axRenderBuiltInAssets;
	using RayTracing = axRenderRayTracing;
	using CreateDesc = axRenderer_CreateDesc;

	~axRenderer();

	static axRenderer* s_instance();

	void setVSync(bool b)	{ _vsync = b; }
	bool vsync() const		{ return _vsync; }

	axRenderApi	api() const { return _api; }
	bool multithread() const { return _multithread; }

	axSPtr<RenderContext>	createRenderContext			(axRenderContext_CreateDesc&		desc) { return onCreateRenderContext(desc); }
	axSPtr<MultiGpuBuffer>	createMultiGpuRenderBuffer	(axRenderGpuBuffer_CreateDesc&		desc);
	axSPtr<GpuBuffer>		createGpuBuffer				(axRenderGpuBuffer_CreateDesc&		desc) { return onCreateGpuBuffer(desc); }
	axSPtr<StorageBuffer>	createStorageBuffer			(axRenderStorageBuffer_CreateDesc&	desc) { return onCreateStorageBuffer(desc); }
	axSPtr<StageBuffer>		createStageBuffer			(axRenderStageBuffer_CreateDesc&	desc) { return onCreateStageBuffer(desc); }

	axSPtr<Shader>			createShaderFromFile		(axStrView filename);
	axSPtr<ShaderPass>		createShaderPass			() { return onCreateShaderPass(); }
	axSPtr<Material>		createMaterial				() { return onCreateMaterial(); }
	axSPtr<MaterialPass>	createMaterialPass			() { return onCreateMaterialPass(); }

	axSPtr<Texture2D>		createTexture2D			(axTexture2D_CreateDesc& desc) { return onCreateTexture2D(desc); }
	axSPtr<Texture2D>		createTexture2D			(const axImage& img, const SamplerState& samplerState = SamplerState());
	axSPtr<Texture2D>		createTexture2DFromFile	(axStrView filename, const SamplerState& samplerState = SamplerState());

	axSPtr<Fence>			createFence				() { return onCreateFence(); }

	BuiltInAssets*			builtInAssets()		{ return _builtInAssets; }

	axFontStyle*			defaultFontStyle()	{ return _defaultFontStyle.ptr(); }
					void	setDefaultFontStyle(axFontStyle* s)	{ _defaultFontStyle.ref(s); }

			axRenderSeqId	getNextRenderSeqId() { return ++_nextRenderSeqId; }

	axRenderRayTracing*		rayTracing() { return _rayTracing.ptr(); }

	struct AdapterInfo {
		axString	adapterName;
		axInt		memorySize = 0;
		bool		multithread = false;

		bool		computeShader = false;
		bool		shaderFloat64 = false;
		axInt		minThreadGroupSize = 1;
	};

	const AdapterInfo&		adapterInfo() const { return _adapterInfo; }
	
	using MemoryInfo = axRenderMemoryInfo;
	void getMemoryInfo(MemoryInfo& info) { onGetMemoryInfo(info); }

friend axSPtr<axRenderer> axRenderer_create(axRenderer_CreateDesc& desc);
friend class axRenderContext;
friend class axRenderContext_EventHandler;
protected:

	axRenderer(CreateDesc& desc);
	virtual void onCreate();
	virtual void onDestroy();

	virtual void onGetMemoryInfo(MemoryInfo& info) { info = {}; }

	virtual axSPtr<RenderContext>			onCreateRenderContext		(axRenderContext_CreateDesc&		desc) = 0;
	virtual axUPtr<axRenderRequest>			onCreateRenderRequest		() = 0;
	virtual axSPtr<axRenderFence>			onCreateFence				() = 0;
	virtual axSPtr<GpuBuffer>				onCreateGpuBuffer			(axRenderGpuBuffer_CreateDesc&		desc) = 0;
	virtual axSPtr<StorageBuffer>			onCreateStorageBuffer		(axRenderStorageBuffer_CreateDesc&	desc) = 0;
	virtual axSPtr<StageBuffer>				onCreateStageBuffer			(axRenderStageBuffer_CreateDesc&	desc) = 0;
	virtual axSPtr<Texture2D>				onCreateTexture2D			(axTexture2D_CreateDesc&			desc) = 0;
	virtual axSPtr<Shader>					onCreateShader				() = 0;
	virtual axSPtr<ShaderPass>				onCreateShaderPass			() = 0;
	virtual axSPtr<axMaterial>				onCreateMaterial			() = 0;
	virtual axSPtr<axMaterialPass>			onCreateMaterialPass		() = 0;

	axUPtr<axRenderRequest> newRequest(axRenderContext* renderContext, const axRect2f& worldRect);
	void freeRequest(axUPtr<axRenderRequest>&& req);
	void submitRequest(axUPtr<axRenderRequest>&& req);

	axSPtr<axRenderRayTracing>	_rayTracing;
	AdapterInfo				_adapterInfo;

	axCondQueue<axRenderRequest>	_pendingRequests;
	axCondQueue<axRenderRequest>	_processingRequests;
	axCondQueue<axRenderRequest>	_freeRequests;

private:
	bool _checkCompletedRequest();
	bool onGpuRenderJob();
	void onRenderThread();

	virtual void onRenderThreadStart() {}

	struct MData {
		bool started = false;
		bool quit = false;
	};

	axCondMutexProtected<MData, true>	_mdata;

	axRenderApi				_api = axRenderApi::Unknown;
	BuiltInAssets*			_builtInAssets = nullptr;
	axSPtr<axFontStyle>		_defaultFontStyle;
	axRenderSeqId			_nextRenderSeqId = 1;
	axThread				_renderThread;

	bool					_multithread : 1;
	bool					_vsync : 1;
};

#define axRenderer_InterfaceFunctions(T) \
	virtual axSPtr<axRenderContext>			onCreateRenderContext		(axRenderContext_CreateDesc&		desc) override; \
	virtual axUPtr<axRenderRequest>			onCreateRenderRequest		() override; \
	virtual axSPtr<axRenderFence>			onCreateFence				() override; \
	virtual axSPtr<axRenderGpuBuffer>		onCreateGpuBuffer			(axRenderGpuBuffer_CreateDesc&		desc) override; \
	virtual axSPtr<axRenderStorageBuffer>	onCreateStorageBuffer		(axRenderStorageBuffer_CreateDesc&	desc) override; \
	virtual axSPtr<axRenderStageBuffer>		onCreateStageBuffer			(axRenderStageBuffer_CreateDesc&	desc) override; \
	virtual axSPtr<axTexture2D>				onCreateTexture2D			(axTexture2D_CreateDesc&			desc) override; \
	virtual axSPtr<axShader>				onCreateShader				() override; \
	virtual axSPtr<axShaderPass>			onCreateShaderPass			() override; \
	virtual axSPtr<axMaterial>				onCreateMaterial			() override; \
	virtual axSPtr<axMaterialPass>			onCreateMaterialPass		() override; \

//-----

#define axRenderer_InterfaceFunctions_Impl(T) \
	axSPtr<axRenderContext>			T##Renderer::onCreateRenderContext		(axRenderContext_CreateDesc&		desc)	{ return ax_new T##RenderContext(this, desc); } \
	axUPtr<axRenderRequest>			T##Renderer::onCreateRenderRequest		()											{ return axUPtr<axRenderRequest>::s_make(ax_new T##RenderRequest()); } \
	axSPtr<axRenderFence>			T##Renderer::onCreateFence				()											{ return ax_new T##Fence(); } \
	axSPtr<axRenderGpuBuffer>		T##Renderer::onCreateGpuBuffer			(axRenderGpuBuffer_CreateDesc&		desc)	{ return ax_new T##GpuBuffer(); } \
	axSPtr<axRenderStorageBuffer>	T##Renderer::onCreateStorageBuffer		(axRenderStorageBuffer_CreateDesc&	desc)	{ return ax_new T##StorageBuffer(desc); } \
	axSPtr<axRenderStageBuffer>		T##Renderer::onCreateStageBuffer		(axRenderStageBuffer_CreateDesc&	desc)	{ return ax_new T##StageBuffer(desc); } \
	axSPtr<axTexture2D>				T##Renderer::onCreateTexture2D			(axTexture2D_CreateDesc&			desc)	{ return ax_new T##Texture2D(desc); } \
	axSPtr<axShader>				T##Renderer::onCreateShader				()											{ return ax_new T##Shader(); } \
	axSPtr<axShaderPass>			T##Renderer::onCreateShaderPass			()											{ return ax_new T##ShaderPass(); } \
	axSPtr<axMaterial>				T##Renderer::onCreateMaterial			()											{ return ax_new T##Material(); } \
	axSPtr<axMaterialPass>			T##Renderer::onCreateMaterialPass		()											{ return ax_new T##MaterialPass(); } \
//-----

axSPtr<axRenderer> axRenderer_create(axRenderer_CreateDesc& desc);
