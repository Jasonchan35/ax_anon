#include <libAnonEditor/App/EditorApp.h>
#include "SceneViewPanel.h"
#include "Tools/SceneView_MoveTool.h"

namespace Anon{ namespace Editor {

SceneViewPanel::ToolBar::ToolBar(axUIView* parent) 
	: Base(parent)
	, debugRenderButton(this, "Debug")
	, vSyncButton(this, "VSync")
	, displayWireframeButton(this, "Wire")
	, enableRayTracingButton(this, "RayTrace")
	, displayIdsLabel(this, "Component ID:")
	, displayIds(this)
	, displayNormalsLabel(this, "Normals: ")
	, displayNormals(this)
	, displayNormalLength(this, 0.1f)
{
	{
		using Flags = axRenderComponentFlags;
		auto f = [&](Flags f) {
			displayIds.addItem(ax_enum_int(f), ax_enum_name(f));
			displayNormals.addItem(ax_enum_int(f), ax_enum_name(f));
		};
		f(Flags::None);
		f(Flags::Point);
		f(Flags::Edge);
		f(Flags::Vertex);
		f(Flags::Primitive);

		displayNormals.selectItemById(ax_enum_int(Flags::None));
		displayNormals.evValueChanged.bind(this, 
			[&](auto& ev) {
				this->_renderComponentNormals = static_cast<Flags>(ev.selectedId);
		});

		displayIds.selectItemById(ax_enum_int(Flags::None));
		displayIds.evValueChanged.bind(this, 
			[&](auto& ev) {
				this->_renderComponentIds = static_cast<Flags>(ev.selectedId);
		});

		vSyncButton.setIsDown(axRenderer::s_instance()->vsync());
		vSyncButton.evClick.bind(this, [this](auto& ev) {
			axRenderer::s_instance()->setVSync(vSyncButton.isDown());
		});
	}

//	displayWireframeButton.setIsDown(true);
//	displayPointIdsButton.setIsDown(true);
//	displayEdgeIdsButton.setIsDown(true);
}

SceneViewPanel::SceneViewPanel(axUIDockZone* zone) 
	: Base(zone)
	, _toolView(this)
	, _toolBar(this)
{
	auto* renderer = axRenderer::s_instance();
	auto* app = EditorApp::s_instance();

	auto assetsDir = app->assetsDir();

	setTitle("Scene View");

//	_testTexture = renderer->createTexture2DFromFile(axString(assetsDir, "/images/uvChecker_BC7.png"));

	axSamplerState samplerState;
	samplerState.filter = axTextureFilter::Point;
	//_testTexture  = renderer->createTexture2DFromFile(axString(assetsDir, "/images/TexFilterTest.png"));
	//_testTexture2 = renderer->createTexture2DFromFile(axString(assetsDir, "/images/TexFilterTest.png"), samplerState);

//	_testTexture = axRenderer::s_instance()->createTexture2DFromFile(axString(assetsDir, "/images/uvCheckerGray.jpg"));
	{
		axImageRGBAb img;
		img.create(256, 256);
		auto painter = img.painter();
		painter.fill(axColorRGBAb::kWhite());

#if 0
		painter.drawAACircle(Vec2(64, 64), 62, axColorRGBAb(255, 0, 0, 128));
		painter.drawAACircleOutline(Vec2(64, 64), 30, axColorRGBAb(0, 0, 0, 128), 5.1f);
#else
		Rect2 rect(10, 8, 200, 100);
		painter.drawAARoundedRect	    (rect,                15, axColorRGBAb(0, 255, 0));
		painter.drawAARoundedRectOutline(rect + Vec2(0, 110), 16, axColorRGBAb(255, 0, 0), 2);
#endif
//		_testImagePainter = renderer->createTexture2D(img, samplerState);
	}

	{
		axImageRGBAb img;
		axInt w = 128;
		axInt h = 128;
		img.create(w, h);

		auto painter = img.painter();
		painter.fill({0, 0, 0, 0});
		
		Rect2 rect(60, 20, 40, 40);
		float radius = 15;

		painter.drawAARoundedRectOutline(rect - Margin2(2), radius, axColorRGBAb(255, 0, 0), 2);

		img.convertToPremultipiedAlpha();

		auto tex = renderer->createTexture2D(img, samplerState);
//		_testSprite.set(nullptr, tex, rect, radius, {0,0}, false);
	}

//	_terrain.createFromImageFile(Vec2d(2048), 500, 5, axString(assetsDir, "/terrains/test001/TerrainHeight_257.png"));
//	_terrain.createFromImageFile(Vec2d(2048), 500, 7, axString(assetsDir, "/terrains/test001/TerrainHeight_1025.png"));

	WaveFrontObjImporter importer;
	switch (0) {
		case 0: break;
		case 1: _editableMesh.edit().createSphere	(50, 8, 16); break;
		case 2: _editableMesh.edit().createCylinder	(50, 20, 4, 6, true, true); break;
		case 3: _editableMesh.edit().createCone		(50, 20, 4, 12, false); break;

		case 101: importer.option.scale = 50; importer.loadFile(_editableMesh, axString(assetsDir, "/models/wavefront_obj/octahedron.obj")); break;
		case 102: importer.option.scale = 50; importer.loadFile(_editableMesh, axString(assetsDir, "/models/wavefront_obj/teapot.obj")); break;
		case 103: importer.option.scale = 50; importer.loadFile(_editableMesh, axString(assetsDir, "/models/wavefront_obj/bunny.obj")); break;
		case 104: importer.option.scale = 50; importer.loadFile(_editableMesh, axString(assetsDir, "/models/wavefront_obj/cow-nonormals.obj")); break;
		case 105: importer.option.scale = 1;  importer.loadFile(_editableMesh, axString(assetsDir, "/models/wavefront_obj/crytek-sponza/sponza.obj")); break;
	}

	_editableMesh.updateFaceNormals();
	_editableMesh.updateFaceVertexNormals(30);

	const auto normalLenght = 10.0f;
	const auto normalColor  = axColor(0.75f, 0.75f, 0);
	auto& stdFontStyle = axUIDefaultAssets::s_instance()->fontStyles.std;

	{
		auto vertexDesc = axRenderVertex_PosUvColor::s_desc();
		//_editableMeshDisplay.edit().createLinesFromVertexNormals(vertexDesc, _editableMesh, normalLenght, normalColor);
		//_editableMeshDisplay.edit().createLinesFromFaceNormals(vertexDesc, _editableMesh, normalLenght, normalColor);
		//_editableMeshDisplay.edit().createLinesFromEdgeNormals(vertexDesc, _editableMesh, normalLenght, normalColor);
		_editableMeshDisplayNormals.edit().createLinesFromFaceVertexNormals(vertexDesc, _editableMesh, normalLenght, normalColor);
		_editableMeshDisplayEdges.edit().createLinesFromEdges(vertexDesc, _editableMesh, normalColor);
		//_editableMeshDisplay.edit().createLinesFromFaceEdges(vertexDesc, _editableMesh, normalColor);
	}

	{
		auto vertexDesc = axRenderVertex_PosUv2Color::s_desc();
		//ids
		_editableMeshDisplayPointIds.edit().createTextFromPointIds(vertexDesc, _editableMesh, stdFontStyle);
		//_editableMeshIdsDisplay.edit().createTextFromEdgeIds(vertexDesc, _editableMesh, stdFontStyle);
		//_editableMeshIdsDisplay.edit().createTextFromFaceIds(vertexDesc, _editableMesh, stdFontStyle);
		//_editableMeshIdsDisplay.edit().createTextFromFaceEdgeIds(vertexDesc, _editableMesh, stdFontStyle);
	}

//------------
	if (_editableMesh.colorSetCount() < 1) {
		_editableMesh.addColorSet(axColor(0.0f, 0.73f, 1.0f));
	}

	_model.edit().createFromEditableMesh(axRenderVertex_PosUvColorNormal::s_desc(), _editableMesh);

//	_model.generateNormal();

	_modelDisplay.edit().createLinesFromVertexNormals(axRenderVertex_PosColor::s_desc(), _model, 10, axColor(0,1,0));

	_grid.edit().createGrid(axRenderVertex_PosColor::s_desc(), 10, 100);

	if (0) {
		float z = 100;
		axBezierCurveHelper<Vec3f>::evaluateCurve(_testCurve, 
													axVec3f(0,   0,   z),
													axVec3f(100, 200, z + 150),
													axVec3f(200,-100, z),
													axVec3f(200, 0,   z));
	}
}

void SceneViewPanel::onRender(axRenderRequest& req) {
	auto* renderer = req.renderer();
	auto& gpuInfo = renderer->adapterInfo();

	if (gpuInfo.computeShader) { // && gpuInfo.shaderFloat64) {
		if (!_testStageBufferDone) {
			using Vertex = axRenderVertex_PosColor;

			if (!_testStageBuffer) {
				axRenderStorageBuffer_CreateDesc  desc;
				desc.bufferSize = 16 * sizeof(Vertex);
				desc.byteStride = sizeof(Vertex);

				auto buffer0	= renderer->createStorageBuffer(desc);
				auto bufferOut	= renderer->createStorageBuffer(desc);

				auto mtl = req.renderer()->createMaterial();
				mtl->setShader(req.builtInAssets()->computeShaders.ComputeTriangles);
//				mtl->setParam(AnonNames::color(), Color(1,0,1,1));

				mtl->setParam(axNameId::s_make("cellSize"),		10.0f);
//				mtl->setParam(axNameId::s_make("testDouble"),	0.1);
//				mtl->setParam(axNameId::s_make("Buffer0"),		buffer0);
				mtl->setParam(axNameId::s_make("BufferOut"),	bufferOut);

				auto tc = axRender_kNumThreads1d;
				req.addComputeCall(AX_LOC, mtl, tc);

				axRenderStageBuffer_CreateDesc stageDesc;
				stageDesc.bufferSize = desc.bufferSize;
				stageDesc.byteStride = desc.byteStride;
				_testStageBuffer = renderer->createStageBuffer(stageDesc);

				req.copyToStageBuffer(_testStageBuffer, bufferOut);

				_testStageBufferFence = req.completedFence();

			} else if (_testStageBufferFence) {
				if (_testStageBufferFence->isDone()) {
					auto compteShaderResult = _testStageBuffer->map();
					// AX_DUMP_HEX(compteShaderResult.span());
					// AX_DUMP_VAR(Vertex::s_desc());

					//auto span = axMutSpan<Vertex>::s_fromByteSpan(compteShaderResult.span());
					//for (auto& v : span) {
					//	AX_DUMP_VAR(v);
					//}
					_testStageBufferFence.unref();
				}
			}
		}
	}

	if (width() <= 0 || height() <= 0)
		return;

	req.world.cameraPos		= _camera.pos();
	req.world.lightPos		= Vec3f(50, 500, 10);
	req.world.lightPower	= 100;
	req.world.lightColor	= ColorRGB(1,1,1);

	auto view = _camera.viewMatrix();
	auto proj = _camera.projMatrix();
		
	//axMat4x4f viewport;
	//viewport.setScale( 1 / width(),  1 / height(), 1 );

	req.world.projection.edit() = proj;
	req.world.modelview.edit()  = view;

	{ // grid
		auto mat = axMat4f::s_translate(axVec3f(0,0,-0.1f));
		//auto mat = axMat4f::s_rotateDegX(90.0f);
		req.world.drawMesh(AX_LOC, _grid, mat, axColor(.4f, .4f, .4f));
	}

	// render world
	{
		auto wireframe = req.debugWireframe.push(_toolBar.displayWireframeButton.isDown());

		if (auto* scnSystem = SCN_System::s_instance()) {
			auto AX_NO_NAME = req.drawComponentIds.push(_toolBar._renderComponentIds);
			auto AX_NO_NAME = req.drawComponentNormals.push(_toolBar._renderComponentNormals);
			auto AX_NO_NAME = req.drawComponentNormalLength.push(_toolBar.displayNormalLength.editingValue());

			scnSystem->render(req);

			auto* app = EditorApp::s_instance();
			for (auto& it : app->selection.current()) {
				if (auto* obj = it.getObject()) {
					obj->drawGizmo(req);
				}
			}
		}

		if (_toolBar.enableRayTracingButton.isDown()) {
			req.world.drawMesh(AX_LOC, _model);

		} else if (_toolBar.displayWireframeButton.isDown()) {
			// display edges
			req.world.drawMesh(AX_LOC, _editableMeshDisplayEdges);

		} else {
			// display model
			auto mtl = req.renderer()->createMaterial();
			mtl->setShader(req.builtInAssets()->shaders.SimpleLighting);
			mtl->setParam(AnonNames::matMvp(),		req.world.mvp());
			mtl->setParam(AnonNames::color(),		axColor::kWhite());
			mtl->setParam(AnonNames::cameraPos(),	_camera.pos());
			mtl->setParam(AnonNames::lightPos(),	axVec3f(10,100,10));
			mtl->setParam(AnonNames::ambient(),		axColor(0.15f, 0.15f, 0.15f));
			mtl->setParam(AnonNames::diffuse(),		axColor(0.4f, 0.4f, 0.4f));
			mtl->setParam(AnonNames::specular(),	axColor(1,1,1));
			mtl->setParam(AnonNames::shininess(),	128.0f);
			req.addDrawCall(AX_LOC, mtl, _model);
		}

		{
			auto mv = req.world.modelview.push();
			auto offset = axVec2f::s_cast(_terrain.terrainSize() / -2);
			mv *= Mat4::s_translate(Vec3f(offset.x, -250.0, offset.y));

			auto mtl = req.renderer()->createMaterial();
			mtl->setShader(req.builtInAssets()->shaders.SimpleLighting);
			mtl->setParam(AnonNames::matMvp(),		req.world.mvp());
			mtl->setParam(AnonNames::color(),		axColor::kWhite());
			mtl->setParam(AnonNames::cameraPos(),	_camera.pos());
			mtl->setParam(AnonNames::lightPos(),	axVec3f(10,100,10));
			mtl->setParam(AnonNames::ambient(),		axColor(0.4f, 0.4f, 0.4f));
			mtl->setParam(AnonNames::diffuse(),		axColor(0.4f, 0.4f, 0.4f));
			mtl->setParam(AnonNames::specular(),	axColor(1,1,1));
			mtl->setParam(AnonNames::shininess(),	128.0f);

			auto viewPos = _camera.pos();
			//viewPos = _testPos;
			_terrain.render(req, axVec3d::s_cast(viewPos), mtl);

			if (_toolBar._renderComponentNormals != axRenderComponentFlags::None) {
				_terrain.renderNormals(req);
			}
		}
	}

	const bool showModelNormal = false;
	if (showModelNormal) { // display model normal
		req.world.drawMesh(AX_LOC, _modelDisplay);
	}

	if (_toolBar._renderComponentNormals != axRenderComponentFlags::None) {
		req.world.drawMesh(AX_LOC, _editableMeshDisplayNormals);
	}

	if (_toolBar._renderComponentIds != axRenderComponentFlags::None) { // display editable mesh ids
		auto mtl = req.renderer()->createMaterial();
		mtl->setShader(req.builtInAssets()->shaders.TextBillboardTexture0);
		mtl->setParam(AnonNames::matMvp(),	req.world.mvp());
		mtl->setParam(AnonNames::color(),	axColor::kWhite());
		mtl->setParam(AnonNames::invViewportSize(), 1.0f / req.viewport().size);
		auto* tex = axFontManager::s_instance()->getGlyphTexture();
		mtl->setParam(AnonNames::glyphTex(), tex);
		req.addDrawCall(AX_LOC, mtl, _editableMeshDisplayPointIds);
	}

	{ // mouse cursor
		const float s = 10;
		req.ui.drawRect(AX_LOC, axRect2f(_lastMousePos - s * 0.5f, axVec2f(s,s) ), axColor(1,0,0));
	}

	if (_testTexture) {
		req.ui.drawTexture(AX_LOC, _testTexture, Rect2(10, 110, 64, 64));
	}
	if (_testTexture2) {
		req.ui.drawTexture(AX_LOC, _testTexture2, Rect2(150, 110, 64, 64));
	}
	if (_testImagePainter) {
		Vec2 s = Vec2::s_cast(_testImagePainter->size() * 1);
		req.ui.drawTexture(AX_LOC, _testImagePainter, Rect2(10, 200, s.x, s.y));
	}
	if (_testSprite.texture()) {
		req.ui.drawTexture(AX_LOC, _testSprite.texture(), Vec2 (300, 110));
		req.ui.drawSprite (AX_LOC, _testSprite, Rect2(300, 250, 200, 200));
	}

	{
		auto& style = axUIDefaultAssets::s_instance()->fontStyles.std;

		axVec2f drawPos(10, 40);
		auto lineHeight = style->font->lineHeight();

		axTempString text;
		text.format("Model:");
		req.ui.drawText(AX_LOC, text, drawPos, style);
		drawPos.y += lineHeight;

		text.format("  Vertex Count: {?}", _model.vertexCount());
		req.ui.drawText(AX_LOC, text, drawPos, style);
		drawPos.y += lineHeight;

		text.format("  Tri Count: {?}", _model.triCount());
		req.ui.drawText(AX_LOC, text, drawPos, style);
		drawPos.y += lineHeight;

		text.format("  Ray = {?}", _camera.getRay(_lastMousePos));
		req.ui.drawText(AX_LOC, text, drawPos, style);
		drawPos.y += lineHeight;
	}

//	req.world.drawCube(AX_LOC, _testPos, axVec3f(1,1,1) * 10, axColor(0,1,0));
	req.world.drawLineStrip(AX_LOC, _testCurve, axColor(0.5f, 1, 0.5f));

	if (_toolBar.debugRenderButton.isDown()) {
		axUIApp::s_instance()->onDebugRender(req);
	}
	req.drawStatistics(Vec2f(width() - 360, 40));
}

void SceneViewPanel::onUpdateLayout() {
	_camera.setViewport(contentRect());
	Base::onUpdateLayout();
}

void SceneViewPanel::onEditorSelectionChanged() {
	auto list = EditorApp::s_instance()->selection.current();
	if (list) {
		auto& item = list.back();
		if (item.get<SCN_Object>()) {
			auto tool = axUPtr_new(SceneView_MoveTool);
			_toolView._tool = ax_move(tool);
			_toolView._tool->_panel = this;
			return;
		}
	}
	_toolView._tool.unref();
}

void SceneViewPanel::onAnonContextChanged(AnonContext::ChangedFlags flags) {
	setNeedToRender();
}

Ray3f SceneViewPanel::getCameraRay(const Vec2f& point) {
	return _camera.getRay(point);
}

inline void SceneViewPanel::moveCamera(const Vec3f& deltaPos) { 
	_camera.move(deltaPos);
}

void SceneViewPanel::onMouseEvent(axUIMouseEvent& ev) {
	using Button = axUIMouseEvent::Button;

	auto ray = getCameraRay(ev.pos);

	if (ev.isLeftClick()) {
		axPlane3f plane(axVec3f(0,1,0), 0);
		axRayCast3f rc;
		if (rc.hitTest(ray, plane, false)) {
			_testPos = rc.result.point;
		}
	} else if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				auto d = ev.deltaPos * 0.01f;
				_camera.orbit(d.x, d.y);
			}break;

			case Button::Middle: {
				auto d = ev.deltaPos * 0.5f;
				_camera.move(d.x, d.y, 0);
			}break;

			case Button::Right: {
				auto d = ev.deltaPos * -0.5f;
				_camera.dolly(d.x + d.y);
			}break;
		}
	//--------
		_lastMousePos = ev.pos;
	}
	setNeedToRender();
}

void SceneViewPanel::onKeyEvent(axUIKeyEvent& ev) {
	using KeyCode = axUIKeyCode;
	using Type = axUIKeyEventType;
	switch (ev.type) {
		case Type::Down: {
			setNeedToRender();
		} break;
	}
}

SceneView_ToolView::SceneView_ToolView(SceneViewPanel* parent)
	: Base(parent)
	, _panel(parent)
{
}

void SceneView_ToolView::onRender(axRenderRequest& req) {
	if (_tool) {
		_tool->_isMouseHover = isMouseHover();
		_tool->onRender(req);
	}
}

void SceneView_ToolView::onMouseEvent(axUIMouseEvent& ev) {
	if (_tool) {
		_tool->onMouseEvent(ev);
	}
}

bool SceneView_ToolView::onHitTest(axUIHitTest& ht) {
	if (_tool) {
		if (_tool->onHitTest(ht)) {
			ht.result.view = this;
			return true;
		}
	}

	return false;
}

}} //namespace

