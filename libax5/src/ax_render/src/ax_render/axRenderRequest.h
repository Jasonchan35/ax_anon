#pragma once

#include "axRenderCommand.h"
#include "buffer/axRenderMultiGpuBuffer.h"
#include "buffer/axRenderStageBuffer.h"
#include "axRenderFence.h"

#include <ax_core/allocator/axLinearAllocator.h>

struct axRenderDrawCallInfo : public axRenderCommonBase  {
	PrimitiveType		primitiveType		= PrimitiveType::Unknown;
	VertexDesc			vertexDesc			= nullptr;
	GpuBuffer*			vertexBuffer		= nullptr;
	axInt				vertexByteOffset	= 0;
	axInt				vertexCount			= 0;

	IndexType			indexType			= IndexType::Unknown;
	GpuBuffer*			indexBuffer			= nullptr;
	axInt				indexByteOffset		= 0;
	axInt				indexCount			= 0;
};

class axRenderRequest : public axTypedBase, public axSListNode<axRenderRequest>, public axRenderCommonBase {
	AX_RTTI_CLASS(axRenderRequest, axNoBase)
public:
	using Command	= axRenderCommand;
	using Names		= axRenderNames;

	axRenderRequest();

	void addComputeCall		(const axSourceLoc& loc, Material* mtl, const axVec3i& threadGroupCount);
	void addDrawCall		(const axSourceLoc& loc, Material* mtl, const Mesh& mesh);
	void addDrawCall		(const axSourceLoc& loc, Material* mtl, const SubMesh& subMesh);
	void addDrawCall		(const axSourceLoc& loc, Material* mtl, PrimitiveType primitiveType, const VertexArray& vertices, const IndexArray* indices);

	void addDrawCall		(const axSourceLoc& loc, Material* mtl, axRenderDrawCallInfo& info) {
		_addDrawCall(loc, mtl, info);
	}

	void _addDrawCall		(const axSourceLoc& loc, Material* mtl, axRenderDrawCallInfo& info);

//	axRenderFence*	addFence();

	void copyToStageBuffer	(StageBuffer* dst, StorageBuffer* src);
	void clearColorAndDepthBuffers(const axColorRGBAf& color, float depth);

	void _swapBuffers		();
	void setViewport		(const Rect2& rect);

	AX_INLINE	Rect2 viewport	() const	{ return _viewport; }

	axRenderer*				renderer() { return axRenderer::s_instance(); }
	axRenderBuiltInAssets*	builtInAssets() { return axRenderBuiltInAssets::s_instance(); }

	axAllocator*			allocator() { return nullptr; }

	struct Layer {
		Layer(axRenderRequest& req) : _req(req) {}

		void reset() {
			projection = modelview = Mat4::kIdentity();
		}

		axSPtr<axMaterial> createSimpleMaterial(Shader* shader, const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());

//		Mat4	mvp() const { return axMat4f::s_scale(1,-1,1) * projection * modelview; }
		Mat4	mvp() const { return projection * modelview; }

		axPushable<Mat4>	modelview  = Mat4::kIdentity();
		axPushable<Mat4>	projection = Mat4::kIdentity();

	protected:
		axRenderRequest& _req;
	};

	struct WorldLayer : public Layer {
		WorldLayer(axRenderRequest& req) : Layer(req) {}

		axSPtr<axMaterial> createMaterial(axShader* shader);
		axSPtr<axMaterial> createMaterial_UnlitTexture0(const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());
		axSPtr<axMaterial> createMaterial_TextBillboardTexture0(const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());
		axSPtr<axMaterial_BlinnPhong> createMaterial_BlinnPhong();

		void drawLine			(const axSourceLoc& loc, const Vec3& pos0, const Vec3& pos1, const axColor& color);
		void drawLines			(const axSourceLoc& loc, axSpan<Vec3> positions, const axColor& color);
		void drawLineStrip		(const axSourceLoc& loc, axSpan<Vec3> positions, const axColor& color);
		void drawCube			(const axSourceLoc& loc, const Vec3& pos, const Vec3& size, const axColor& color);
		void drawMesh			(const axSourceLoc& loc, const Mesh& mesh, const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());

		void drawMeshVertexNormals(const axSourceLoc& loc, const Mesh& mesh, float normalLength = 10.0f, const axColorPair& color = axColor::kMagenta());

		void drawText			(const axSourceLoc& loc, axStrView text, const Vec3& pos, const axFontStyle* style = nullptr);

		axVec3f		cameraPos {0,0,0};
		axColorRGBf	lightColor {1,1,1};
		axVec3f		lightPos {10, 10, 10};
		float		lightPower = 40.0f;
	};
	
	struct UILayer : public Layer {
		UILayer(axRenderRequest& req) : Layer(req) {}

		axSPtr<Material> createMaterial_UITexture0		(                     const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());
		axSPtr<Material> createMaterial_UITexture1		(Texture2D* mainTex0, const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());
		axSPtr<Material> createMaterial_UITextTexture0	(Texture2D* glyphTex, const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());

		void drawLine				(const axSourceLoc& loc, const Vec2& pos0, const Vec2& pos1, const axColor& color);
		void drawLines				(const axSourceLoc& loc, axSpan<Vec2> positions, const axColor& color);
		void drawLineStrip			(const axSourceLoc& loc, axSpan<Vec2> positions, const axColor& color);

		void drawRect				(const axSourceLoc& loc, const Rect2& rect, const axColor& color);
		void drawRectBorder			(const axSourceLoc& loc, const Rect2& rect, const Margin2& border, const axColor& color);

		void drawText				(const axSourceLoc& loc, axStrView text, const Vec2& pos, const axFontStyle* style = nullptr);

		void drawTexture			(const axSourceLoc& loc, Texture2D* texture, const Vec2&  pos,  const Rect2& uv = Rect2(0,0,1,1), const axColor& color = axColor::kWhite());
		void drawTexture			(const axSourceLoc& loc, Texture2D* texture, const Rect2& rect, const Rect2& uv = Rect2(0,0,1,1), const axColor& color = axColor::kWhite());

		void drawColorMaskTexture	(const axSourceLoc& loc, Texture2D* texture, const Rect2& rect, const Rect2& uv,
									 const axColor& color0,
									 const axColor& color1,
									 const axColor& color2,
									 const axColor& color3);

		void drawBorderTexture		(const axSourceLoc& loc, Texture2D* texture, const Vec2&  pos,
									 const Margin2& border, bool hasCenter, 
									 const Rect2& uv = Rect2(0,0,1,1),
									 const axColor& color = axColor::kWhite());

		void drawBorderTexture		(const axSourceLoc& loc, Texture2D* texture, const Rect2& rect,
									 const Margin2& border, bool hasCenter,
									 const Rect2& uv = Rect2(0,0,1,1),
									 const axColor& color = axColor::kWhite());

		void drawSprite				(const axSourceLoc& loc, Sprite& sprite, const Vec2&  pos,  const Rect2& uv = Rect2(0,0,1,1), const axColor& color = axColor::kWhite());
		void drawSprite				(const axSourceLoc& loc, Sprite& sprite, const Rect2& rect, const Rect2& uv = Rect2(0,0,1,1), const axColor& color = axColor::kWhite());

		void drawMesh				(const axSourceLoc& loc, const Mesh&    mesh, const Mat4& matrix = Mat4::kIdentity(), const axColor& color = axColor::kWhite());

		void setProjectMatrix		(const Vec2& size);
	};

	WorldLayer	world;
	UILayer		ui;


	axPushable<axRenderComponentFlags>	drawComponentIds	 = axRenderComponentFlags::None;
	axPushable<axRenderComponentFlags>	drawComponentNormals = axRenderComponentFlags::None;
	axPushable<float>					drawComponentNormalLength = 16.0f;

	axPushable<bool>	debugWireframe = false;
	axPushable<Rect2>	scissorRect = Rect2(0,0,0,0);

	float	getWorldToScreenScale(const Vec3& atPos) const;

	template<class T>
	T* newCommand() {
	#if 1  // use linear allocator
		auto p = axUPtr_new_(&_linearAllocator, T);
	#else
		auto p = axUPtr_new(T);
	#endif
		_commandQueue.append(p.ptr());
		return p.detach();
	}

	template<class T>
	T* newComputeCommand() {
	#if 1  // use linear allocator
		auto p = axUPtr_new_(&_linearAllocator, T);
	#else
		auto p = axUPtr_new(T);
	#endif
		_computeCommandQueue.append(p.ptr());
		return p.detach();
	}

	void updateGpuResource();

	axSList<Command>&	commandQueue() & { return _commandQueue; }
	axSList<Command>&	computeCommandQueue() & { return _computeCommandQueue; }

	axInt64				renderSeqId() const { return _renderSeqId; }
	axRenderContext*	renderContext()		{ return _renderContext; }
	axRenderFence*		completedFence()	{ return _completedFence; }

	struct Statistics {
		bool	show = false;
		axInt	drawcalls = 0;
		axInt	drawTriangles = 0;
		axInt	drawLines = 0;
		axInt	drawPoints = 0;
		axInt	vertexCount = 0;
		axInt	renderBufferUploadBytes = 0;
	};
	Statistics statistics;
	void drawStatistics(const Vec2& pos);

friend class axRenderer;
protected:
	void reset(axRenderSeqId requestId, axRenderContext* renderContext, const axRect2f& worldRect);
	void gpuEnded();

	virtual void onGpuEnded() {}

	axLinearAllocator			_linearAllocator;

	axRenderer*					_renderer = nullptr;
	axSPtr<axRenderContext>		_renderContext;

	axRenderSeqId				_renderSeqId = 0;
	Mesh						_tmpMesh;

	BuiltInAssets*				_builtInAssets = nullptr;

	axSPtr<axRenderGpuBuffer>	_inlineDrawVertexGpuBuffer;
	axSPtr<axRenderGpuBuffer>	_inlineDrawIndexGpuBuffer;

	axByteArray			_inlineDrawVertices;
	axByteArray			_inlineDrawIndices;

	Rect2				_viewport {0,0,0,0};

	axSList<Command>		_commandQueue;
	axSList<Command>		_computeCommandQueue;
	axSPtr<axRenderFence>	_completedFence;
};
