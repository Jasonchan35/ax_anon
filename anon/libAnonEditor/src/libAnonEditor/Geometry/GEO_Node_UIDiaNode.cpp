#include "GEO_Node_UIDiaNode.h"
#include "../EditorUI/Diagram/EdUIDiagram.h"
#include "../EditorUI/EdUICommon.h"

namespace Anon { namespace Editor {

EdUIDiagram* GEO_Node_UIDiaNode::diagram() {
	return ax_type_cast<EdUIDiagram>(Base::diagram());
}

GEO_Node* GEO_Node_UIDiaNode::diaNode() { return ax_type_cast<GEO_Node>(Base::diaNode()); }

GEO_Modifier* GEO_Node_UIDiaNode::modifierNode() {
	return ax_type_cast<GEO_Modifier>(Base::diaNode());
}

GEO_Node_UIDiaNode::GEO_Node_UIDiaNode(CreateDesc& desc) 
	: Base(desc) 
	, _bypassButton(&titleBar(), "Bypass")
	, _isOutputButton(&titleBar(), "OUT")
{
	_bypassButton.setVisible(modifierNode());

	_bypassButton.evClick.bind(this, [this](auto& ev) {
		setBypass(_bypassButton.isDown());
	});

	_isOutputButton.evClick.bind(this, [this](auto& ev) {
		setAsOutput();
	});
}

void Editor::GEO_Node_UIDiaNode::onMouseEvent(axUIMouseEvent& ev) {
//	using Type		= axUIMouseEvent::Type;
//	using Button	= axUIMouseEvent::Button;
//	using Modifier	= axUIMouseEvent::Modifier;

	if (ev.isRightClick()) {
		auto* menu = ax_new EdUIContextMenu(this);
		menu->addItem("Set as Output", this, [this](){ setAsOutput(); });
		menu->setWorldPos(ev.worldPos);
		menu->setWindowActive(true);
		menu->updateLayout();
	}

	Base::onMouseEvent(ev);
}

void GEO_Node_UIDiaNode::onRender(axRenderRequest& req) {
	if (auto* dia = geoDiagram()) {
		_isOutputButton.setIsDown(dia->outputNode() == diaNode());
	}

	if (auto* p = modifierNode()) {
		_bypassButton.setIsDown(p->bypass());
	}

	Base::onRender(req);
}

void GEO_Node_UIDiaNode::setAsOutput() {
	if (auto* node = diaNode()) {
		node->setAsOutputNode();
	}
}

void GEO_Node_UIDiaNode::setBypass(bool b) {
	if (auto* node = modifierNode()) {
		node->setBypass(b);
	}
}

GEO_Diagram* GEO_Node_UIDiaNode::geoDiagram() {
	if (auto* view = diagram()) {
		return ax_type_cast<GEO_Diagram>(view->diagram());
	}
	return nullptr;
}


}}
