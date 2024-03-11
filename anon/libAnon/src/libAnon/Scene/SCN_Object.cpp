#include "SCN_Object.h"
#include "SCN_System.h"
#include "SCN_Component.h"

#include "../Core/AnonContext.h"

namespace Anon {
SCN_Object::SCN_Object(SCN_System& system) {
	_init();
}

SCN_Object::SCN_Object(SCN_Object* parent) {
	if (!parent) {
		parent = system()->rootObject();
		if (!parent) return;
	}
	parent->addChild(this);
	_init();
}

SCN_Object* SCN_Object::s_newFromJson(axJsonReader& rd, SCN_Object* parent) {
	auto* obj = ax_new SCN_Object(parent);
	rd.beginObject();

	while (!rd.endObject()) {
		if (rd.isMember("__name__")) {
			obj->setName(rd.valueStrView());
			rd.next();
			continue;
		}
			
		if (rd.beginArray("__children__")) {
			while (!rd.endArray()) {
				s_newFromJson(rd, obj);
			}
			continue;
		}
			
		if (rd.beginArray("__components__")) {
			while (!rd.endArray()) {
				SCN_Component::s_newFromJson(rd, obj);
			}
			continue;
		}

		obj->readJsonProps(rd);
	}
	return nullptr;
}

void SCN_Object::onJsonWriter(axJsonWriter& wr) {
	auto AX_NO_NAME = wr.object();

	wr.writeMember("__name__", this->name());
	if (_components) {
		auto AX_NO_NAME = wr.array("__components__");
		for (auto& comp : _components) {
			comp.onJsonWriter(wr);
		}
	}
	if (_children) {
		auto AX_NO_NAME = wr.array("__children__");
		for (auto& child : _children) {
			child.onJsonWriter(wr);
		}
	}

	writeJsonProps(wr);
}

SCN_System* SCN_Object::system() {
	return SCN_System::s_instance();
}

void SCN_Object::_init() {
}

void SCN_Object::onDrawGizmo(axRenderRequest& req) {
	Base::onDrawGizmo(req);
	req.world.drawText(AX_LOC, ax_typeof(this)->name(), worldPos());
	req.world.drawCube(AX_LOC, worldPos(), Vec3f(5), Color(1,1,0));
}

void SCN_Object::onSetComputeNeeded() {
	Base::onSetComputeNeeded();
	for (auto& c : _children) {
		c.setComputeNeeded();
	}
}

void SCN_Object::onCompute() {
	Base::onCompute();

	auto m = Mat4f::s_TRS_Deg(_translate, _rotate, _scale);
	if (_parent) {
		ax_const_cast(_localToWorldMatrix) = _parent->localToWorldMatrix() * m;
	} else {
		ax_const_cast(_localToWorldMatrix) = m;
	}
}

SCN_Object::~SCN_Object() {
	clearChildren();
	clearComponents();
	removeFromParent();
}

void SCN_Object::addChild(SCN_Object* c) {
	if (!c) return;
	c->removeFromParent();
	c->_parent = this;
	_children.append(c);
}

SCN_Object* SCN_Object::findChild(axStrView inPath) {
	auto path = inPath;
	auto* obj = this;

	axTempString tmpName;

	while (path) {
		auto pair = path.splitByChar('/');

		bool found = false;
		for (auto& c : obj->_children) {
			c.name().getString(tmpName);
			if (tmpName != pair.first) continue;
			if (!pair.second) return &c;

			found = true;
			path = pair.second;
			obj = &c;
			break;
		}

		if (!found) return nullptr;
	}
	return nullptr;
}

void SCN_Object::clearChildren() {
	while (auto* p = _children.tail()) {
		_children.remove(p);
		p->_parent = nullptr;
		ax_delete(p);
	}	
}

void SCN_Object::removeFromParent() {
	if (_parent) {
		_parent->_children.remove(this);
		_parent = nullptr;
	}
}

SCN_Component* SCN_Object::findComponent(const axType* type) {
	for (auto& c : _components) {
		if (ax_typeof(c)->isKindOf(type))
			return &c;
	}
	return nullptr;
}

SCN_Component* SCN_Object::newComponent(const axSourceLoc& loc, axStrView type) {
	return system()->newComponent(loc, type, this);
}

void SCN_Object::addComponent(SCN_Component* c) {
	if (!c) return;
	c->removeFromObject();
	c->_sceneObject = this;
	_components.append(c);
}

void SCN_Object::removeComponent(SCN_Component* c) {
	if (!c) return;
	_components.remove(c);
	c->_sceneObject = nullptr;
}

void SCN_Object::clearComponents() {
	while (auto* p = _components.tail()) {
		_components.remove(p);
		p->_sceneObject = nullptr;
		ax_delete(p);
	}	
}

} //namespace
