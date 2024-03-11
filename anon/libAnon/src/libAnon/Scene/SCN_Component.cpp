#include "SCN_Component.h"
#include "SCN_Object.h"
#include "SCN_System.h"

namespace Anon {

SCN_Component::SCN_Component(SCN_Object* object) {
	if (object) {
		object->addComponent(this);
	}
}

axNameId SCN_Component::sceneObjectName() const {
	return _sceneObject ? _sceneObject->name() : axNameId();
}

void SCN_Component::removeFromObject() {
	if (_sceneObject) {
		_sceneObject->removeComponent(this);
		_sceneObject = nullptr;
	}
}

void SCN_Component::onJsonWriter(axJsonWriter& wr) {
	auto AX_NO_NAME = wr.object();
	wr.writeMember("__type__", ax_typeof(this)->name());
	wr.writeMember("__name__", this->name());
	Base::onJsonWriter(wr);
}

SCN_Component* SCN_Component::s_newFromJson(axJsonReader& rd, SCN_Object* parentObject) {
	if (!parentObject) return nullptr;

	SCN_Component* comp = nullptr;
	rd.beginObject();
	while (!rd.endObject()) {
		if (rd.isMember("__type__")) {
			if (comp) {
				rd.error("\"__type__\" has been specified");
				throw axError_Undefined(AX_LOC);
			}
			comp = parentObject->newComponent(AX_LOC, rd.valueStrView());
			rd.next();
			continue;
		}

		if (!comp) {
			rd.error("missing \"__type__\" for component");
			throw axError_Undefined(AX_LOC);
		}

		if (rd.isMember("__name__")) {
			comp->setName(rd.valueStrView());
			rd.next();
			continue;
		}

		comp->readJsonProps(rd);
	}

	return nullptr;
}

SCN_System* SCN_Component::system() {
	return _sceneObject ? _sceneObject->system() : nullptr;
}

SCN_RendererBase::SCN_RendererBase(SCN_Object* object)
	: Base(object) 
{
	system()->_addRenderer(this);
}

SCN_RendererBase::~SCN_RendererBase() {
	axDListNode<SCN_RendererBase>::removeFromList();
}

} //namespace
