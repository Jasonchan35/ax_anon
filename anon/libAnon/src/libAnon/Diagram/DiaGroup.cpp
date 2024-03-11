#include "DiaGroup.h"

namespace Anon {

DiaGroup::DiaGroup(IDiaSystem* system, DiaGroup* parent)
	: Base(system, parent)
{

}

DiaNode* DiaGroup::addNode(DiaNode* node) {
	node->_diagram = _diagram;
	return _nodes.append(node);
}

DiaNode* DiaGroup::findNode(axStrView name) {
	return findNode(axNameId::s_make(name));
}

DiaNode* DiaGroup::findNode(axNameId name) {
	for (auto& p : _nodes) {
		if (p.name() == name)
			return &p;
	}
	return nullptr;
}

bool DiaGroup::onReadJsonProp(axJsonReader& rd) {
	bool ret = Base::onReadJsonProp(rd);
	if (rd.beginArray("nodes")) {
		while (!rd.endArray()) {
			s_newNodeFromJson(this, rd);
		}
		ret = true;
	}
	return ret;
}

void DiaGroup::onWriteJsonProp(axJsonWriter& wr) const {
	Base::onWriteJsonProp(wr);
	wr.writeMember("nodes", _nodes);
}


} //namespace