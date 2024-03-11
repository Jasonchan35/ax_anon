#include "DiaGroup.h"
#include "DiaNode.h"
#include "DiaSystem.h"

namespace Anon {

DiaNode::DiaNode(IDiaSystem* system, DiaGroup* parent) {
	connNode._diaNode = this;
	_system = system;
	if (parent) {
		parent->addNode(this);
	}
}

DiaNode::~DiaNode() {
	ListNode::removeFromList();
}

void DiaNode::onJsonWriter(axJsonWriter& wr) const {
	wr.writeMember("__type__", ax_typeof(this)->name());
	wr.writeMember("__name__", _name);
	wr.writeMember("__diaPos__", _diaPos);
	Base::onJsonWriter(wr);
}

DiaNode* DiaNode::s_newNodeByTypeName(DiaGroup* parent, axStrView typeName) {
	if (!parent) return nullptr;
	auto* sys = parent->system();
	auto* node = sys->newNode(AX_LOC, typeName, sys, parent);
	return node;
}

DiaNode* DiaNode::s_newNodeFromJson(DiaGroup* parent, axJsonReader& rd) {
	if (!parent) return nullptr;

	DiaNode* node = nullptr;
	rd.beginObject();
	while (!rd.endObject()) {
		if (rd.isMember("__type__")) {
			if (node) {
				rd.error("\"__type__\" has been specified");
				throw axError_Undefined(AX_LOC);
			}

			node = s_newNodeByTypeName(parent, rd.valueStrView());
			rd.next();
			continue;
		}

		if (!node) {
			rd.error("missing \"__type__\" for component");
			throw axError_Undefined(AX_LOC);
		}

		if (rd.isMember("__name__")) {
			node->setName(rd.valueStrView());
			rd.next();
			continue;
		}

		if (rd.readMember("__diaPos__", node->_diaPos))
			continue;

		if (node->readJsonProps(rd))
			continue;

		auto* type = ax_typeof(node);
		rd.warning("read prop[{?}.{?}] not found", type->name(), rd.valueStrView());
		rd.skipValue();
	}

	return node;
}

void DiaNode::onCompute() {
	Base::onCompute();

	for (auto& inPlug : connNode.inSock.plugs()) {
		inPlug.connection().transferValue();
	}
}

void DiaNode::onSetComputeNeeded() {
	Base::onSetComputeNeeded();

	for (auto& outPlug : connNode.outSock.plugs()) {
		if (auto* dstNode = outPlug.inPlug().diaNode()) {
			dstNode->setComputeNeeded();
		}
	}
}

} //namespace