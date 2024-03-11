#include "axUIDiaProp.h"
#include "axUIDiaNode.h"

axUIDiaProp::axUIDiaProp(axUIView* parent, Node* node, axStrView name, axStrView displayName, bool isInput)
	: Base(parent)
	, _nameBox(this)
	, _socket(this, this, isInput)
{
	_node = node;

//	setAsFirstSibling();
	layout.setExpandW().setFitContentH();
	setPadding({10, 2});
	setDisplayName(displayName);
	setPropName(name);
	if (!isInput) {
		_nameBox.layout.setAlignX(AlignX::Right);
	}
}

bool axUIDiaProp::isInput() const {
	return _socket.isInput();
}

void axUIDiaProp::onUpdateConnectionPosition() {
	_socket.onUpdateConnectionPosition();
}

void axUIDiaProp::setPropName(axStrView propName) {
	_propName = propName;
}

void axUIDiaProp::setDisplayName(axStrView s) {
	_displayName = s;
	_nameBox.setText(_displayName);
}

axStrView axUIDiaProp::nodeName() const {
	return _node ? _node->name() : axStrView_make("null");
}

void axUIDiaProp::onUpdateLayout() {
	auto rc = contentRect();
	_nameBox.setLayoutResultRect(rc);

	auto pos = Vec2(isInput() ? 0 : _rect.size.x, _rect.size.y / 2);
	pos -= _socket.size() / 2;
	_socket.setPos(pos);
}

void axUIDiaProp::onFormat(axFormat& f) const {
	f.append("axUIDiaProp(", nodeName(), ".", propName(), ")");
}

axUIDiaPropGroup::axUIDiaPropGroup(axUIView* parent) 
	: Base(parent) 
{
	layout.setFitContentH();
}

axUIDiaProp* axUIDiaPropGroup::findProp(axStrView name) {
	for (auto& p_ : children()) {
		auto* p = ax_type_cast<axUIDiaProp>(&p_);
		if (!p) continue;
		if (p->propName() == name)
			return p;
	}
	return nullptr;
}

void axUIDiaPropGroup::onUpdateConnectionPosition() {
	for (auto& e : _children) {
		if (auto* prop = ax_type_cast<axUIDiaProp>(&e)) {
			prop->onUpdateConnectionPosition();
		}
	}
}
