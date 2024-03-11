#include "axRenderRequest.h"
#include "axRenderContext.h"
#include "shader/axShader.h"
#include "font/axFontManager.h"
#include "texture/atlas/axTextureSprite.h"

axRenderRequest::axRenderRequest() 
	: world(*this)
	, ui(*this)
	, _linearAllocator(ax_default_allocator(), 2 * 1024 * 1024)
{
	_tmpMesh.setIsInlineDraw(true);
}

axSPtr<axMaterial> axRenderRequest::Layer::createSimpleMaterial(Shader* shader, const Mat4& matrix, const axColor& color) {
	auto mtl = _req._renderer->createMaterial();
	mtl->setShader(shader);
	mtl->setParam(Names::matMvp(), mvp() * matrix);
	mtl->setParam(Names::color(),  color);
	return mtl;
}

axSPtr<axMaterial> axRenderRequest::WorldLayer::createMaterial(axShader* shader) {
	auto mtl = _req._renderer->createMaterial();
	mtl->setShader(shader);
	mtl->setParam(Names::matMvp(), mvp());
	return mtl;
}

axSPtr<axMaterial> axRenderRequest::WorldLayer::createMaterial_UnlitTexture0(const Mat4& matrix, const axColor& color) {
	return createSimpleMaterial(_req._builtInAssets->shaders.UnlitTexture0, matrix, color);
}

axSPtr<axMaterial> axRenderRequest::WorldLayer::createMaterial_TextBillboardTexture0(const Mat4& matrix, const axColor& color) {
	auto mtl = _req.renderer()->createMaterial();
	mtl->setShader(_req.builtInAssets()->shaders.TextBillboardTexture0);
	mtl->setParam(Names::matMvp(),	mvp());
	mtl->setParam(Names::color(),	axColor::kWhite());
	mtl->setParam(Names::invViewportSize(), 1.0f / _req.viewport().size);

	auto* tex = axFontManager::s_instance()->getGlyphTexture();
	mtl->setParam(Names::glyphTex(), tex);

	return mtl;
}

axSPtr<axMaterial_BlinnPhong> axRenderRequest::WorldLayer::createMaterial_BlinnPhong() {
	auto mtl = _req.renderer()->createMaterial();
	mtl->setShader(_req.builtInAssets()->shaders.BlinnPhong);
	mtl->setParam(Names::matMvp(),		mvp());
	mtl->setParam(Names::cameraPos(),	cameraPos);
	mtl->setParam(Names::lightPos(),	lightPos);
	mtl->setParam(Names::lightPower(),	lightPower);
	mtl->setParam(Names::lightColor(),	lightColor);

	return axMaterial_BlinnPhong::s_cast(mtl);
}

void axRenderRequest::WorldLayer::drawLine(const axSourceLoc& loc, const Vec3& pos0, const Vec3& pos1, const axColor& color) {
	Vec3 positions[] = {pos0, pos1};
	drawLines(loc, positions, color);
}

void axRenderRequest::WorldLayer::drawLines(const axSourceLoc& loc, axSpan<Vec3> positions, const axColor& color) {
	_req._tmpMesh.edit().createLines(axRenderVertex_PosColor::s_desc(), positions, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::WorldLayer::drawLineStrip(const axSourceLoc& loc, axSpan<Vec3> positions, const axColor& color) {
	_req._tmpMesh.edit().createLineStrip(axRenderVertex_PosColor::s_desc(), positions, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::WorldLayer::drawCube(const axSourceLoc& loc, const Vec3& pos, const Vec3& size, const axColor& color) {
	if (color.a == 0 || size == Vec3(0))
		return;

	_req._tmpMesh.edit().createCube(axRenderVertex_PosColor::s_desc(), pos, size, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::WorldLayer::drawMesh(const axSourceLoc& loc, const Mesh& mesh, const Mat4& matrix, const axColor& color) {
	auto mtl = createMaterial_UnlitTexture0(matrix, color);
	_req.addDrawCall(loc, mtl, mesh);
}

void axRenderRequest::WorldLayer::drawMeshVertexNormals(const axSourceLoc& loc, const Mesh& mesh, float normalLength, const axColorPair& color) {
	using Vertex = axRenderVertex_PosColor;
	_req._tmpMesh.edit().createLinesFromVertexNormals(Vertex::s_desc(), mesh, normalLength, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::WorldLayer::drawText(const axSourceLoc& loc, axStrView text, const Vec3& pos, const axFontStyle* style) {
	auto color   = style ? style->color : axColor::kWhite();
	
	_req._tmpMesh.edit().createTextBillboard(axRenderVertex_PosUv2Color::s_desc(), text, pos, style);
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.TextBillboardTexture0, Mat4::kIdentity(), color);
	mtl->setParam(Names::glyphTex(), axFontManager::s_instance()->getGlyphTexture());
	_req.addDrawCall(loc, mtl, _req._tmpMesh);
}

axSPtr<axMaterial> axRenderRequest::UILayer::createMaterial_UITexture0(const Mat4& matrix, const axColor& color) {
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.UITexture0, matrix, color);
	return mtl;
}

axSPtr<axMaterial> axRenderRequest::UILayer::createMaterial_UITexture1(Texture2D* mainTex0, const Mat4& matrix, const axColor& color) {
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.UITexture1, matrix, color);
	mtl->setParam(Names::mainTex0(), mainTex0);
	return mtl;
}

axSPtr<axMaterial> axRenderRequest::UILayer::createMaterial_UITextTexture0(Texture2D* glyphTex, const Mat4& matrix, const axColor& color) {
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.UITextTexture0, matrix, color);
	mtl->setParam(Names::glyphTex(), glyphTex);
	return mtl;
}

void axRenderRequest::UILayer::drawRect(const axSourceLoc& loc, const Rect2& rect, const axColor& color) {
	if (color.a == 0 || rect.size == Vec2(0)) return;
	_req._tmpMesh.edit().createRect(axRenderVertex_PosUvColor::s_desc(), rect, Rect2(0,0,1,1), color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawRectBorder(const axSourceLoc& loc, const Rect2& rect, const Margin2& border, const axColor& color) {
	if (color.a == 0 || rect.size == Vec2(0) || border == Margin2(0)) return;
	_req._tmpMesh.edit().createBorderRect(axRenderVertex_PosUvColor::s_desc(), rect, border, Rect2(0,0,1,1), Margin2(0,0,0,0), color, false);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawLine(const axSourceLoc& loc, const Vec2& pos0, const Vec2& pos1, const axColor& color) {
	Vec2 positions[] = {pos0, pos1};
	drawLines(loc, positions, color);
}

void axRenderRequest::UILayer::drawLines(const axSourceLoc& loc, axSpan<Vec2> positions, const axColor& color) {
	_req._tmpMesh.edit().createLines(axRenderVertex_PosColor::s_desc(), positions, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawLineStrip(const axSourceLoc& loc, axSpan<Vec2> positions, const axColor& color) {
	_req._tmpMesh.edit().createLineStrip(axRenderVertex_PosColor::s_desc(), positions, color);
	drawMesh(loc, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawTexture(const axSourceLoc& loc, Texture2D* texture, const Vec2& pos, const Rect2& uv, const axColor& color) {
	if (!texture) return;
	Rect2 rect(pos, axVec2f::s_cast(texture->size()));
	drawTexture(loc, texture, rect, uv, color);
}

void axRenderRequest::UILayer::drawTexture(const axSourceLoc& loc, Texture2D* texture, const Rect2& rect, const Rect2& uv, const axColor& color) {
	if (color.a == 0 || rect.size == Vec2(0)) return;
	_req._tmpMesh.edit().createRect(axRenderVertex_PosUvColor::s_desc(), rect, uv, color);

	auto mtl = createMaterial_UITexture1(texture, Mat4::kIdentity(), color);
	_req.addDrawCall(loc, mtl, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawColorMaskTexture(
	const axSourceLoc& loc,
	Texture2D* texture,
	const Rect2& rect,
	const Rect2& uv,
	const axColor& color0,
	const axColor& color1,
	const axColor& color2,
	const axColor& color3) 
{
	_req._tmpMesh.edit().createRect(axRenderVertex_PosUvColor::s_desc(), rect, uv, axColor::kWhite());
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.UIColorMaskTexture);
	mtl->setParam(Names::mainTex0(), texture);
	mtl->setParam(Names::color0(), color0);
	mtl->setParam(Names::color1(), color1);
	mtl->setParam(Names::color2(), color2);
	mtl->setParam(Names::color3(), color3);

	_req.addDrawCall(loc, mtl, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawBorderTexture(const axSourceLoc& loc, Texture2D* texture, const Vec2& pos, 
												 const Margin2& border, bool hasCenter,
												 const Rect2& uv, const axColor& color)
{
	if (!texture) return;
	Rect2 rect(pos, axVec2f::s_cast(texture->size()));
	drawBorderTexture(loc, texture, rect, border, hasCenter, uv, color);
}

void axRenderRequest::UILayer::drawBorderTexture(const axSourceLoc& loc, Texture2D* texture, const Rect2& rect, 
												 const Margin2& border, bool hasCenter,
												 const Rect2& uv, const axColor& color) 
{
	if (color.a == 0 || rect.size == Vec2(0)) return;
	if (border == Margin2(0)) {
		return drawTexture(loc, texture, rect, uv, color);
	}
	auto uvBorder = border / Vec2::s_cast(texture->size());
	_req._tmpMesh.edit().createBorderRect(axRenderVertex_PosUvColor::s_desc(), rect, border, uv, uvBorder, color, hasCenter);

	auto mtl = createMaterial_UITexture1(texture, Mat4::kIdentity(), color);
	_req.addDrawCall(loc, mtl, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawSprite(const axSourceLoc& loc, Sprite& sprite, const Vec2& pos, const Rect2& uv, const axColor& color) {
	drawSprite(loc, sprite, Rect2(pos, sprite.size()), uv, color);
}

void axRenderRequest::UILayer::drawSprite(const axSourceLoc& loc, Sprite& sprite, const Rect2& rect, const Rect2& uv, const axColor& color) {
	auto* tex = sprite.texture();
	auto texUv = sprite.toTextureSpace(uv);
	drawBorderTexture(loc, tex, rect, sprite.border(), sprite.hasCenter(), texUv, color);
}

void axRenderRequest::UILayer::drawText(const axSourceLoc& loc, axStrView text, const Vec2& pos, const axFontStyle* style) {
	auto color   = style ? style->color : axColor::kWhite();
	
	if (style) {
		_req._tmpMesh.edit().createText(axRenderVertex_Pos::s_desc(), text, pos, style);
		auto bgColor = style->bgColor;
		if (bgColor.a > 0) {
			auto bbox = _req._tmpMesh.calcBoundingBox();
			auto rc = bbox.toRect2() + style->bgPadding;
			drawRect(loc, rc, bgColor);
		}		
	}

	_req._tmpMesh.edit().createText(axRenderVertex_PosUvColor::s_desc(), text, pos, style);
	auto mtl = createSimpleMaterial(_req._builtInAssets->shaders.UIText, Mat4::kIdentity(), color);
	mtl->setParam(Names::glyphTex(), axFontManager::s_instance()->getGlyphTexture());	
	_req.addDrawCall(loc, mtl, _req._tmpMesh);
}

void axRenderRequest::UILayer::drawMesh(const axSourceLoc& loc, const Mesh& mesh, const Mat4& matrix, const axColor& color) {
	auto mtl = createMaterial_UITexture0(matrix, color);
	_req.addDrawCall(loc, mtl, mesh);
}

void axRenderRequest::UILayer::setProjectMatrix(const Vec2& size) {
	projection = Mat4::s_ortho(0, size.x, size.y, 0, -1000, 1000);
}

void axRenderRequest::addDrawCall(const axSourceLoc& loc, Material* mtl, const Mesh& mesh) {
	for (auto& sm : mesh.subMeshes()) {
		addDrawCall(loc, mtl, sm);
	}
}

void axRenderRequest::addDrawCall(const axSourceLoc& loc, Material* mtl, const SubMesh& subMesh) {
	addDrawCall(loc, mtl, subMesh.primitiveType(), subMesh.vertices, &subMesh.indices);
}

void axRenderRequest::addDrawCall(const axSourceLoc& loc, Material* mtl, PrimitiveType primitiveType, const VertexArray& vertices, const IndexArray* indices) {
	axRenderDrawCallInfo info;
	info.primitiveType = primitiveType;
	info.vertexDesc = vertices.vertexDesc();

	if (vertices.size() <= 0) return;

	if (vertices.isInlineDraw()) {
		info.vertexBuffer		= _inlineDrawVertexGpuBuffer;
		auto offset = ax_multiple_of(_inlineDrawVertices.sizeInBytes(), axInt(256)); // vulkan on mac require 256
		info.vertexByteOffset	= offset;;
		info.vertexCount		= vertices.size();

		_inlineDrawVertices.reserve(offset + vertices.byteSpan().size());
		_inlineDrawVertices.resize(offset, axTag::NoInit);
		_inlineDrawVertices.appendRange(vertices.byteSpan());

	} else {
		info.vertexBuffer		= ax_const_cast(vertices).uploadToGpu(*this);
		info.vertexByteOffset	= 0;
		info.vertexCount		= vertices.size();
	}

	if (indices) {
		info.indexType = indices->indexType();
		if (info.indexType != IndexType::Unknown) {
			if (indices->size() <= 0)
				return;

			if (indices->isInlineDraw()) {
				info.indexBuffer		= _inlineDrawIndexGpuBuffer;
				info.indexByteOffset	= _inlineDrawIndices.sizeInBytes();
				info.indexCount			= indices->size();
				_inlineDrawIndices.appendRange(indices->byteSpan());

			} else {
				info.indexBuffer		= ax_const_cast(indices)->uploadToGpu(*this);
				info.indexByteOffset	= 0;
				info.indexCount			= indices->size();
			}
		}
	}

	_addDrawCall(loc, mtl, info);
}

void axRenderRequest::_addDrawCall(const axSourceLoc& loc, Material* mtl, axRenderDrawCallInfo& info) {
	AX_ZoneScoped;

	if (!mtl) throw axError_Undefined(AX_LOC);
	if (!mtl->shader()) throw axError_Undefined(AX_LOC);

	if (info.vertexDesc == nullptr) return;
	if (info.vertexCount <= 0) return;
	if (!info.vertexBuffer) throw axError_Undefined(AX_LOC);

	if (info.indexType != IndexType::Unknown) {
		if (info.indexCount <= 0) return;
		if (!info.indexBuffer) throw axError_Undefined(AX_LOC);
	}

	mtl->shader()->checkAcceptVertexDesc(info.vertexDesc);

	auto* cmd = newCommand<axRenderCommand_DrawCall>();
	cmd->drawCall.scissorWorldRect = scissorRect;

	auto& o = cmd->drawCall;
	o.debugLoc			= loc;
	o.debugWireframe	= debugWireframe;

	o.primitiveType		= info.primitiveType;

	o.material			= mtl;
	mtl->onUpdateGpuResource(*this);

	o.vertexBuffer		= info.vertexBuffer;
	o.vertexByteOffset	= info.vertexByteOffset;
	o.vertexDesc		= info.vertexDesc;
	o.vertexCount		= info.vertexCount;

	o.indexBuffer		= info.indexBuffer;
	o.indexByteOffset	= info.indexByteOffset;
	o.indexType			= info.indexType;
	o.indexCount		= info.indexCount;

// statistics
	axInt elementCount = o.isIndexEnabled() ? o.indexCount : o.vertexCount;
	switch (o.primitiveType) {
		case PrimitiveType::Points:    statistics.drawPoints    += elementCount;     break;
		case PrimitiveType::Lines:     statistics.drawLines     += elementCount / 2; break;
		case PrimitiveType::Triangles: statistics.drawTriangles += elementCount / 3; break;
	}

	statistics.vertexCount += elementCount;
	statistics.drawcalls++;
}

//axRenderFence* axRenderRequest::addFence() {
//	auto* cmd = newCommand<axRenderCommand_Fence>();
//	auto f = _renderer->createFence();
//	cmd->fence = f;
//	return f;
//}

void axRenderRequest::copyToStageBuffer(StageBuffer* dst, StorageBuffer* src) {
	if (!dst || !src) throw axError_Undefined(AX_LOC);
	if (dst->bufferSize() != src->bufferSize()) throw axError_Undefined(AX_LOC);

	auto* cmd = newComputeCommand<axRenderCommand_CopyToStageBuffer>();
	cmd->dst = dst;
	cmd->src = src;
}

void axRenderRequest::addComputeCall(const axSourceLoc& loc, Material* mtl, const axVec3i& threadGroupCount) {
	if (!renderer()->adapterInfo().computeShader) {
		throw axError_Undefined(AX_LOC);
		return;
	}

	auto* cmd = newComputeCommand<axRenderCommand_ComputeCall>();
	cmd->computeCall.threadGroupCount = threadGroupCount;
	cmd->computeCall.material = mtl;
	mtl->onUpdateGpuResource(*this);
}

void axRenderRequest::drawStatistics(const Vec2& pos) {
	Vec2 drawPos = pos;

	axRenderer::MemoryInfo memoryInfo;
	renderer()->getMemoryInfo(memoryInfo);

	auto ad = renderer()->adapterInfo();
	auto memorySizeInMB   = ad.memorySize / (1024 * 1024);
	auto memoryBudgetInMB = memoryInfo.budget / (1024 * 1024);
	auto memoryUsageInMB  = memoryInfo.used / (1024 * 1024);

	axTempString text;
	text.appendFormat("{?} ({?}MB)\n", ad.adapterName, memorySizeInMB);
	text.appendFormat("                   memory: {?}/{?}MB\n", memoryUsageInMB, memoryBudgetInMB);
	text.appendFormat("                    vsync: {?}\n", renderer()->vsync());
	text.appendFormat("              renderSeqId: {?}\n", _renderSeqId);
	text.appendFormat("                drawcalls: {?}\n", statistics.drawcalls);
	text.appendFormat("                triangles: {?}\n", statistics.drawTriangles);
	text.appendFormat("                    lines: {?}\n", statistics.drawLines);
	text.appendFormat("                   points: {?}\n", statistics.drawPoints);
	text.appendFormat("              vertexCount: {?}\n", statistics.vertexCount);
	text.appendFormat("  renderBufferUploadBytes: {?}\n", statistics.renderBufferUploadBytes);
	text.appendFormat("  inlineDrawVertices.Size: {?}\n", _inlineDrawVertices.size());
	text.appendFormat("   inlineDrawIndices.Size: {?}\n", _inlineDrawIndices.size());

	ui.drawText(AX_LOC, text, drawPos);
}

void axRenderRequest::reset(axRenderSeqId renderSeqId, axRenderContext* renderContext, const axRect2f& worldRect) {
	statistics = Statistics();

	_linearAllocator.reset();

	_renderContext = renderContext;
	_renderer = renderContext->renderer();
	_completedFence = _renderer->createFence();

	_renderSeqId = renderSeqId;
	_commandQueue.clear();

	_completedFence = _renderer->createFence();

	world.reset();
	ui.reset();
	ui.setProjectMatrix(worldRect.size);

	_viewport = axRect2f(-1, -1, 2, 2);

	scissorRect.edit() = worldRect;

	_builtInAssets = _renderer->builtInAssets();

	if (!_inlineDrawVertexGpuBuffer) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Vertex;
		_inlineDrawVertexGpuBuffer = _renderer->createGpuBuffer(desc);
	}

	if (!_inlineDrawIndexGpuBuffer) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Index;
		_inlineDrawIndexGpuBuffer = _renderer->createGpuBuffer(desc);
	}

	_inlineDrawVertices.clearAndReserve(2 * 1024 * 1024);
	_inlineDrawIndices.clearAndReserve(2 * 1024 * 1024);

//	_drawCalls.clear();
}

void axRenderRequest::gpuEnded() {
	AX_ZoneScoped;

	onGpuEnded();

	_computeCommandQueue.clear();
	_commandQueue.clear();
	_renderContext.unref();
}

void axRenderRequest::updateGpuResource() {
	if (_inlineDrawVertices.size()) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Vertex;
		_inlineDrawVertexGpuBuffer->createWithData(desc, _inlineDrawVertices);
	}
	if (_inlineDrawIndices.size()) {
		axRenderGpuBuffer_CreateDesc desc;
		desc.type = axRenderBufferType::Index;
		_inlineDrawIndexGpuBuffer->createWithData(desc, _inlineDrawIndices);
	}
}

void axRenderRequest::clearColorAndDepthBuffers(const axColorRGBAf& color, float depth) {
	auto* cmd = newCommand<axRenderCommand_ClearColorAndDepthBuffers>();
	cmd->color = color;
	cmd->depth = depth;
}

void axRenderRequest::_swapBuffers() {
	newCommand<axRenderCommand_SwapBuffers>();
}

void axRenderRequest::setViewport(const Rect2& rect) {
	auto* cmd = newCommand<axRenderCommand_SetViewport>();
	cmd->viewport = rect;
	_viewport = rect;
}

float axRenderRequest::getWorldToScreenScale(const Vec3& atPos) const {
	auto up = world.modelview->dirY();
	auto mat = world.mvp();
	auto v0 = mat.mulPoint(atPos);
	auto v1 = mat.mulPoint(atPos + up);
	auto o = 1 / (v0.distance(v1) * _viewport.h);
	return o;
}
