#pragma once

#include "DiaNode.h"

namespace Anon {

class DiaGroup : public DiaNode {
	AX_RTTI_CLASS(DiaGroup, DiaNode);
public:
	DiaGroup(IDiaSystem* system, DiaGroup* parent);

	DiaNode* addNode(DiaNode* node);
	void clearNodes() { _nodes.clear(); }

	axDList<DiaNode>::Enumerator		nodes()			{ return _nodes.each(); }
	axDList<DiaNode>::ConstEnumerator	nodes() const	{ return _nodes.each(); }

	DiaNode* findNode(axStrView name);
	DiaNode* findNode(axNameId name);

	DiaNode* newNodeByTypeName(axStrView typeName) { return s_newNodeByTypeName(this, typeName); }

	bool onReadJsonProp(axJsonReader& rd) override;
	void onWriteJsonProp(axJsonWriter& wr) const override;

private:
	axDList<DiaNode>	_nodes;
};

} //namespace
