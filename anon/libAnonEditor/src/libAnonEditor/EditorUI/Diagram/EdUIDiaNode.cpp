#include "EdUIDiaNode.h"
#include "EdUIDiagram.h"

namespace Anon { namespace Editor {

EdUIDiaNode::EdUIDiaNode(CreateDesc& desc) 
	: Base(desc.parent)
{
	if (auto* node = desc.diaNode) {
		setLinkedObject(desc.diaNode);
		setName(node->name().toString());
		setPos(node->diaPos());
	}
}

EdUIDiagram* EdUIDiaNode::diagram()	{
	return ax_type_cast<EdUIDiagram>(Base::diagram());
}

void EdUIDiaNode::onDiaNodePositionChanged() {
	if (auto* node = diaNode()) {
		node->setDiaPos(this->pos());
	}
}

}}