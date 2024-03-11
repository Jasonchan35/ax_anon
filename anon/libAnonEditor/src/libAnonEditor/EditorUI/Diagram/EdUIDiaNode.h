#pragma once

namespace Anon { namespace Editor {

class EdUIDiagram;
class EdUIDiaNode : public axUIDiaNode {
	AX_RTTI_CLASS(EdUIDiaNode, axUIDiaNode)
public:
	struct CreateDesc {
		axUIDiaGroup*	parent  = nullptr;
		DiaNode*		diaNode = nullptr;
	};

	EdUIDiaNode(CreateDesc& desc);
	
	virtual void onDiaNodePositionChanged() override;

	DiaNode*		diaNode	()	{ return linkedObject<DiaNode>(); }
	EdUIDiagram*	diagram	();
};

}}