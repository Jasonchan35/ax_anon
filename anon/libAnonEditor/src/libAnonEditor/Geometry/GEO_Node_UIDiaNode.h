#pragma once
#include "../EditorUI/Diagram/EdUIDiaNode.h"
#include "../EditorUI/EdUICommon.h"

namespace Anon { namespace Editor {

class GEO_Node_UIDiaNode : public EdUIDiaNode {
	AX_RTTI_CLASS(GEO_Node_UIDiaNode, EdUIDiaNode)
public:
	GEO_Node_UIDiaNode(CreateDesc& desc);

	virtual void onMouseEvent(axUIMouseEvent& ev) override;

	virtual void onRender(axRenderRequest& req) override;

	void setAsOutput();
	void setBypass(bool b);

	GEO_Diagram*	geoDiagram();
	EdUIDiagram*	diagram();
	GEO_Node*		diaNode();

	GEO_Modifier* modifierNode();

private:

	EdUIToggleButton	_bypassButton;
	EdUIToggleButton	_isOutputButton;
};

}}