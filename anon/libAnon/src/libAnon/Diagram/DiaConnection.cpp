#include "DiaConnection.h"
#include "DiaNode.h"

namespace Anon {

void DiaInPlug::connect(Socket* sock, const AnonFieldPath& fieldPath) {
	this->removeFromList();
	_fieldPath = fieldPath;
	_sock = sock;
	if (!sock) return;
	sock->_plugs.append(this);
	if (auto* node = sock->diaNode()) {
		node->setComputeNeeded();
	}
}

void DiaInSocket::disconnect(const AnonFieldPath& path) {
	axArray<DiaConnection*, 16> connToDelete;

	for (auto& p : _plugs) {
		if (p.fieldPath() == path) {
			connToDelete.emplaceBack(&p.connection());
		}
	}

	for (auto* conn : connToDelete) {
		ax_delete(conn);
	}
}

void DiaOutPlug::connect(Socket* sock, const AnonFieldPath& fieldPath) {
	this->removeFromList();
	_fieldPath = fieldPath;
	_sock = sock;
	if (!sock) return;
	sock->_plugs.append(this);
	if (auto* node = sock->diaNode()) {
		node->setComputeNeeded();
	}
}

void DiaInPlug::getString(axIString& outStr) const {
	auto* node = diaNode();
	_fieldPath.toString(outStr, node);
}

AnonFieldValuePtr DiaInPlug::getFieldValuePtr() {
	auto* node = diaNode();
	if (!node) return nullptr;
	return node->getFieldValuePtr(_fieldPath);
}

void DiaInPlug::onFormat(axFormat& f) const {
	axTempString str;
	getString(str);
	f << str;
}

AnonFieldValuePtr DiaOutPlug::getFieldValuePtr() {
	auto* node = diaNode();
	if (!node) return nullptr;
	return node->getFieldValuePtr(_fieldPath);
}

void DiaOutPlug::onFormat(axFormat& f) const {
	axTempString str;
	getString(str);
	f << str;
}

void DiaOutPlug::getString(axIString& outStr) const {
	auto* node = diaNode();
	_fieldPath.toString(outStr, node);
}

bool DiaConnection::transferValue() {
	auto i = inPlug.getFieldValuePtr();
	auto o = outPlug.getFieldValuePtr();

	auto srcNode = outPlug.diaNode();
	if (!srcNode) return false;
	srcNode->compute();

	if (!i.fieldInfo || !o.fieldInfo) return false;
	auto* type = i.fieldInfo->type();
	if (!type) return false;

	if (o.fieldInfo->type() != type)
		return false;

	type->voidPtrAssign(i.value, o.value);
	return true;
}

void DiaConnection::onFormat(axFormat& f) const {
	f.format("{?} -> {?}", outPlug, inPlug);
}

}