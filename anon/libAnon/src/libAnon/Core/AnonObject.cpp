#include "AnonObject.h"
#include "AnonContext.h"

namespace Anon {

void AnonObjectId::onFormat(axFormat& f) const {
	f.format("[ObjectId {?}]", _v);
}

AnonObject* AnonObjectId::getObject() const {
	auto& mgr = AnonContext::s_instance()->objectManager;
	return mgr.findObject(*this);
}

AnonObject::AnonObject() 
	: _computeNeeded(true)
{
	if (auto* anon = AnonContext::s_instance()) {
		anon->objectManager.onNewObject(this);
		anon->markChangedFlags(AnonContext::ChangedFlags::NewObject);
	}
}

AnonObject::~AnonObject() {
	if (auto* anon = AnonContext::s_instance()) {
		if (anon->inEditor()) {
			anon->markChangedFlags(AnonContext::ChangedFlags::DeleteObject);

			for (auto& link : Linkable_each()) {
				if (auto* view = ax_type_cast<axObject>(link.owner())) {
					view->onLinkedObjectDestroy(this);
				}
			}
		}
		anon->objectManager.onDeleteObject(this);
	}
}

AnonObject* AnonObject::s_find(AnonObjectId objectId) {
	return AnonContext::s_instance()->objectManager.findObject(objectId);
}

AnonFieldValuePtr AnonObject::getFieldValuePtr(const AnonFieldPath& fieldPath) {
	auto* type = ax_typeof(this);
	char* ptr = reinterpret_cast<char*>(this);
	const axFieldInfo* field = nullptr;

	auto pi = fieldPath.pathIndex.span();
	while (pi.size()) {
		field = type->findField(pi[0]);
		if (!field) return nullptr;

		ptr += field->offset();
		type = field->type();

		pi = pi.sliceFrom(1);
	}

	return 	AnonFieldValuePtr(field, ptr);
}

AnonFieldValuePtr AnonObject::getFieldValuePtr(axStrView fieldPath) {
	return getFieldValuePtr(getFieldPath(fieldPath));
}

void AnonObject::onJsonReader(axJsonReader& rd) {
	readJsonProps(rd);
}

void AnonObject::onJsonWriter(axJsonWriter& wr) const {
//	wr.writeMember("__name__", name());
	writeJsonProps(wr);
}

bool AnonObject::readJsonProps(axJsonReader& rd) {
	return onReadJsonProp(rd);
}

void AnonObject::writeJsonProps(axJsonWriter& wr) const {
	onWriteJsonProp(wr);
}

bool AnonObject::onReadJsonProp(axJsonReader& rd) { return false; }

void AnonObject::setFieldValue(axStrView fieldPath, axStrView newValue) {
	auto* t = ax_typeof(this);
	t->setValueByPath(this, fieldPath, newValue);
	setComputeNeeded();
	evValueChanged.emit();
}

AnonFieldPath AnonObject::getFieldPath(axStrView path) {
	auto* type = ax_typeof(this);

	const axFieldInfo* fi = nullptr;
	AnonFieldPath o;
	auto next = path;
	while (next) {
		if (!type) {
			AX_LOG_ERROR("invalid field path [{?}]", path);
			throw axError_Undefined(AX_LOC);
		}

		auto pair = next.splitByChar('.');
		auto& fieldName = pair.first;
		next = pair.second;

		axInt arrIndex = -1;
		auto arr = fieldName.splitByChar('[');
		if (arr.second) {
			fieldName = arr.first;

			auto& tmp = arr.second;
			if (tmp.back() != ']') {
				AX_LOG_ERROR("invalid field path [{?}]", path);
				throw axError_Undefined(AX_LOC);
			}

			tmp = tmp.sliceLess(1);
			if (!tmp.tryParse(arrIndex)) {
				AX_LOG_ERROR("invalid field path [{?}]", path);
				throw axError_Undefined(AX_LOC);
			}
		}

		fi = type->findField(fieldName);
		if (!fi) {
			AX_LOG_ERROR("invalid field path [{?}]", path);
			throw axError_Undefined(AX_LOC);
		}

		o.pathIndex.emplaceBack(fi->index());
		if (arrIndex >= 0) {
			o.pathIndex.emplaceBack(arrIndex);
		}

		type = fi->type();
	}

	o.fieldInfo = fi;
	return o;
}

void AnonObject::setToDefaultValue() {
	auto* t = ax_typeof(this);
	for (auto& f : t->fields()) {
		f->setToDefaultValue(this);
	}
	setComputeNeeded();
	evValueChanged.emit();
}

void AnonObject::onWriteJsonProp(axJsonWriter& json_) const {}

void AnonObject::onSetComputeNeeded() {
	AnonContext::s_instance()->setComputeNeeded();
	_calledBaseComputeNeeded = true;
}

void AnonFieldPath::toString(axIString& outStr, const AnonObject* obj) const {
	outStr.clear();
	if (!obj) return;

	obj->name().appendToString(outStr);

	auto* type = ax_typeof(obj);
	auto fields = type->fields();

	auto index = pathIndex[0];
	if (index >= fields.size()) return;

	outStr.append('.', fields[index]->name());
}

} //namespace