#include "SceneView_MoveTool.h"
#include <libanonEditor/App/EditorApp.h>
#include <libanonEditor/Panels/SceneViewPanel/SceneViewPanel.h>

namespace Anon { namespace Editor {

SceneView_MoveTool::SceneView_MoveTool() {
	const float coneHeight = 40;
	const float coneRadius = 12;
	const axInt coneColumns = 16;

	const float cyclinderHeight = 220;
	const float cyclinderRadius = 2;

	_axisHeight = cyclinderHeight + coneHeight;

	axRenderMesh	cone;
	axRenderMesh	cyclinder;

	using Vertex = axRenderVertex_PosColor;

	cyclinder.edit().createCylinder(Vertex::s_desc(), cyclinderHeight, cyclinderRadius, 1, coneColumns, true, true);
	cyclinder.transform(axMat4f::s_translate({0, cyclinderHeight * 0.5f, 0}));

	cone.edit().createCone(Vertex::s_desc(), coneHeight, coneRadius, 1, coneColumns, true);
	cone.transform(axMat4f::s_translate({0, cyclinderHeight + coneHeight * 0.5f, 0}));

	_axisY.mesh.combine(cone);
	_axisY.mesh.combine(cyclinder);

	_axisX.mesh.copy(_axisY.mesh);
	_axisX.mesh.transform(axMat4f::s_rotateDegZ(-90.0f));

	_axisZ.mesh.copy(_axisY.mesh);
	_axisZ.mesh.transform(axMat4f::s_rotateDegX(90.0f));

	_axisX.init(axColor(1,0,0), axVec3f(1,0,0));
	_axisY.init(axColor(0,1,0), axVec3f(0,1,0));
	_axisZ.init(axColor(0,0,1), axVec3f(0,0,1));
}

void SceneView_MoveTool::_updatePos() {
	Vec4f pos(0);

	auto list = EditorApp::s_instance()->selection.current();
	for (auto& item : list) {
		if (auto* obj = ax_type_cast<SCN_Object>(item.getObject())) {
			pos += Vec4f(obj->worldPos(), 1);
		}
	}		
	_pos = pos.homogenize();
}


void SceneView_MoveTool::onRender(axRenderRequest& req) {
	_updatePos();

	auto screenScale = req.getWorldToScreenScale(_pos);
	const auto s = _axisHeight * screenScale;

	if (!isMouseHover()) {
		_hoverAxis = nullptr;
	}

	_axisX.onRender(*this, req, _pos, _pos + axVec3f(s,0,0), screenScale);
	_axisY.onRender(*this, req, _pos, _pos + axVec3f(0,s,0), screenScale);
	_axisZ.onRender(*this, req, _pos, _pos + axVec3f(0,0,s), screenScale);

//	req.world.drawCube(AX_LOC, _debugPoint, axVec3f(5), axColor(1,0,0));
}

void SceneView_MoveTool::Axis::init(const axColor& color_, const axVec3f dir_) {
	color = color_;
	dir = dir_;
}

bool SceneView_MoveTool::onHitTest(axUIHitTest& ht) {
	auto ray = getCameraRay(ht.pos);

	if (_axisX.hitTest(ray)) {
		return true;
	} else if (_axisY.hitTest(ray)) {
		return true;
	} else if (_axisZ.hitTest(ray)) {
		return true;
	}

	return false;
}

void SceneView_MoveTool::onMouseEvent(axUIMouseEvent& ev) {
	using Type = axUIMouseEvent::Type;

	if (ev.isLeftClick()) {
		selectAxis(hitAxis(ev.pos));
		return;
	}
	
	switch (ev.type) {
		case Type::Move: {
			_hoverAxis = hitAxis(ev.pos);
		}break;

		case Type::DragBegin: {
			selectAxis(hitAxis(ev.pos));
			_pointOnAxis(_lastDragPos, ev.pos);
		}break;

		case Type::DragEnd: {
			axVec3f outPos;
			if (_pointOnAxis(outPos, ev.pos)) {
				onMoved(outPos, false);
			}
			selectAxis(nullptr);
		}break;

		case Type::Dragging: {
			axVec3f outPos;
			if (_pointOnAxis(outPos, ev.pos)) {
				onMoved(outPos, true);
			}
		}break;
	}
}

bool SceneView_MoveTool::_pointOnAxis(axVec3f& outPos, const axVec2f& pt) {
	if (!_selectedAxis) return false;

	axLine3f line(_selectedAxis->pos0, _selectedAxis->pos1);
	auto ray = getCameraRay(pt);

	if (!ray.getClosestPoint(outPos, line, -ax_inf, ax_inf))
		return false;

	return true;
}

void SceneView_MoveTool::onMoved(const axVec3f& newPos, bool isPreview) {
	auto deltaPos = newPos - _lastDragPos;

	auto* mgr = AppCommandManager::s_instance();
	mgr->undoPreview();

	auto cmd = axUPtr_new(SetValueCommand());
	cmd->fieldPath = "translate";

	auto* app = EditorApp::s_instance();
	auto list = app->selection.current();
	for (auto& item : list) {
		if (auto* obj = ax_type_cast<SCN_Object>(item.getObject())) {
			auto invMat = obj->parentToWorldMatrix().inverse3x3();
			auto d = invMat.mulVector(deltaPos);
			auto oldValue = obj->translate();
			auto newValue = oldValue + d;
//			obj->setTranslate(t);

			cmd->objects.emplaceBack(obj->objectId());
			axJsonUtil::stringify(cmd->oldValue, oldValue);
			axJsonUtil::stringify(cmd->newValue, newValue);
		}
	}
	mgr->addCommand(cmd.detach(), isPreview);
	// _lastDragPos = newPos;
}

SceneView_MoveTool::Axis* SceneView_MoveTool::hitAxis(const axVec2f& pt) {
	auto ray = getCameraRay(pt);

	if (_axisX.hitTest(ray)) return &_axisX;
	if (_axisY.hitTest(ray)) return &_axisY;
	if (_axisZ.hitTest(ray)) return &_axisZ;

	return nullptr;
}

void SceneView_MoveTool::selectAxis(Axis* s) {
	if (s == _selectedAxis) return;
	_selectedAxis = s;
}

void SceneView_MoveTool::Axis::onRender(SceneView_MoveTool& owner, axRenderRequest& req, const axVec3f& pos0_, const axVec3f& pos1_, float screenScale_) {
	pos0 = pos0_;
	pos1 = pos1_;
	screenScale = screenScale_;

	auto selected = owner._selectedAxis == this;
	auto hover    = owner._hoverAxis == this;

	auto drawColor = color;
	if (selected) drawColor = axColor(1,1,0);
	if (hover) drawColor = axColor(1,0,1);

//	req.world.drawLine(AX_LOC, pos0, pos1, drawColor);
	//req.world.drawCube(AX_LOC, pos1, Vec3f(8), drawColor);

//	auto AX_NO_NAME = req.debugWireframe.push(true);

	auto mtl = req.renderer()->createMaterial();
	mtl->setShader(req.builtInAssets()->shaders.Gizmo);
	mtl->setParam(AnonNames::matMvp(),   req.world.mvp() * axMat4f::s_translateScale(pos0, screenScale));
	mtl->setParam(AnonNames::color(), drawColor);
	req.addDrawCall(AX_LOC, mtl, mesh);
}

bool SceneView_MoveTool::Axis::hitTest(const axRay3f& ray) {
	axCapsule3f capsule(pos0, pos1, 24 * screenScale);

	axRayCast3f rc;
	return rc.hitTest(ray, capsule);
}

}} //namespace
