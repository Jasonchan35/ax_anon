#include "AnonObjectBinding.h"

namespace Anon { namespace Editor {

AnonObjectBinding::~AnonObjectBinding() {
	clearObjects();
}

bool AnonObjectBinding::addObject(AnonObject* obj) {
	if (!obj || objects.contains(obj->objectId()))
		return false;

	objects.emplaceBack(obj->objectId());
	obj->evValueChanged.bind(this, [this]() { 	evValueChanged.emit(); });
	evValueChanged.emit();
	return true;
}

void AnonObjectBinding::clearObjects() {
	for (auto& oid : objects) {
		if (auto* obj = AnonObject::s_find(oid)) {
			obj->evValueChanged.unbindByObject(this);
		}
	}
}

void AnonObjectBinding::onGetObjectInfo(axIString& info) {
	if (objects.size() != 1) {
		info.format("Multiple {?}", objects.size());
		return;
	}
	if (auto* obj = AnonObject::s_find(objects[0])) {
		obj->getObjectInfo(info);
	}
}

void AnonObjectBinding::setToDefaultValue() {
	for (auto& oid : objects) {
		if (auto* obj = AnonObject::s_find(oid)) {
			obj->setToDefaultValue();
		}
	}
	evValueChanged.emit();
}

void AnonObjectBinding::_setComputeNeeded() {
	AnonContext::s_instance()->setComputeNeeded();
}

}}
