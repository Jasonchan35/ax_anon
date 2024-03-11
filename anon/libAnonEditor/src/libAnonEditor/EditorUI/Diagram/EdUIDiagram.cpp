#include <libanonEditor/App/EditorApp.h>
#include <libanonEditor/EditorUI/EdUICommon.h>
#include <libanonEditor/Panels/DiagramPanel/DiagramPanel.h>

#include "EdUIDiagram.h"
#include "EdUIDiaNode.h"
#include "EdUIDiaGroup.h"
#include "EdUIDiaConnection.h"

namespace Anon { namespace Editor {

EdUIDiagram::EdUIDiagram(DiagramPanel* parent) 
	: Base(parent)
	, _panel(parent)
{
	_diagram.setOwner(this);

	evSelectionChanged.bind(this, [this]() {
		axArray<AnonObject*, 128>	list;
		for (auto& t : selection()) {
			if (auto* item = ax_type_cast<EdUIDiaNode>(&t)) {
				list.emplaceBack(item->diaNode());
			}
		}
		EditorApp::s_instance()->selection.select(list);
	});

}

void EdUIDiagram::setDiagram(Diagram* dia) {
	_diagram = dia;
	reloadContent();
}

EdUIDiaNode* EdUIDiagram::getViewFromNode(AnonObject* node) {
	if (!node) return nullptr;
	for (auto& link : node->Linkable_each()) {
		if (auto* view = link.owner<EdUIDiaNode>()) {
			if (view->diagram() == this)
				return view;
		}
	}
	return nullptr;
}

void EdUIDiagram::onEditorSelectionChanged() {
	clearSelection(false);

	auto* app = EditorApp::s_instance();

	for (auto& s : app->selection.current()) {
		auto* node = ax_type_cast<DiaNode>(s.getObject());
		if (!node) continue;

		auto* view = getViewFromNode(node);
		if (!view) continue;

		addSelection(view, false);
	}
}

void EdUIDiagram::onReloadContent() {
	clearChildNodes();
	if (!_diagram) return;
	reload_group(root(), _diagram);
	reload_connections(_diagram);

	setNeedToMeasure();
}

void EdUIDiagram::reload_group(axUIDiaGroup* view, DiaGroup* group) {
	auto* app =	EditorApp::s_instance();
	auto& ed = app->modules.editorUI;

	axInt i = 0;
	for (auto& node : group->nodes()) {
		if (auto* subGroup = ax_type_cast<DiaGroup>(&node)) {
			EdUIDiaGroup::CreateDesc desc;
			desc.parent  = view;
			desc.diaGroup = subGroup;

			auto* p = ax_new EdUIDiaGroup(desc);
			p->setTitle(node.name().toString());
			reload_group(p, subGroup);

		} else {
			EdUIDiaNode::CreateDesc desc;
			desc.parent  = view;
			desc.diaNode = &node;

			auto* p = ed.createDiaNode(AX_LOC, nullptr, desc);
			reload_node(p, &node);
		}

		i++;
	}
}

void EdUIDiagram::reload_node(axUIDiaNode* view, DiaNode* node) {
	view->clearAllProps();

	auto* app =	EditorApp::s_instance();
	if (app->selection.has(node)) {
		addSelection(view, false);
	}

	auto* type = ax_typeof(node);
	for (auto& field : type->fields()) {
		if (field->findAttr<OutputAttribute>()) {
			view->newOutputProp(field->name(), field->name());
		} else {
			view->newInputProp(field->name(), field->name());
		}
		continue;
	}
}

void EdUIDiagram::reload_connections(DiaNode* node) {
	if (auto* group = ax_type_cast<DiaGroup>(node)) {
		for (auto& c : group->nodes()) {
			reload_connections(&c);
		}
	}

	auto* inView = getViewFromNode(node);
	if (!inView) return;

	for (auto& inPlug : node->connNode.inSock.plugs()) {
		auto& conn = inPlug.connection();
		auto& outPlug = conn.outPlug;

		auto* outView = getViewFromNode(outPlug.diaNode());	if (!outView) return;

		auto* inField  = inPlug.fieldInfo();	if (!inField ) return;
		auto* outField = outPlug.fieldInfo();	if (!outField) return;

		auto* inPropView  = inView->findInputProp(inField->name());		if (!inPropView ) return;
		auto* outPropView = outView->findOutputProp(outField->name());	if (!outPropView) return;

		auto* connView = ax_new EdUIDiaConnection(&inPropView->socket(), &outPropView->socket());
		connView->setLinkedObject(&conn);
	}
}

inline axUIDiaConnection* EdUIDiagram::onNewConnectionView(axUIDiaSocket* parent, axUIDiaSocket* target) {
	return ax_new EdUIDiaConnection(parent, target);
}

bool EdUIDiagram::onWillAcceptConnection(axUIDiaProp* src, axUIDiaProp* dst) {
	return true;
}

void EdUIDiagram::_createNode(axStrView typeName) {
	auto* dia = diagram();
	if (!dia) return;

	auto* node = dia->newNodeByTypeName(typeName);
	auto mousePos = axUIInputManager::s_instance()->mouseWorldPos() - worldPos();

	node->setName(typeName);
	node->setDiaPos(mousePos + Vec2(-50, -50));

	EdUIDiaNode::CreateDesc desc;
	desc.parent  = root();
	desc.diaNode = node;

	auto* app =	EditorApp::s_instance();
	auto& ed = app->modules.editorUI;
	auto* nodeView = ed.createDiaNode(AX_LOC, nullptr, desc);

	reload_node(nodeView, node);
}

void EdUIDiagram::onMouseEvent(axUIMouseEvent& ev) {
//	using Type = axUIMouseEvent::Type;
//	using Button = axUIMouseEvent::Button;

	if (ev.isRightClick()) {
		auto* menu = ax_new EdUIContextMenu(this);

		for (auto& t : GEO_System::s_instance()->nodeTypes()) {
			menu->addItem(t->name(),
				this, [this, t]()
			{
				this->_createNode(t->name());
			});
		}	

		menu->setWorldPos(ev.worldPos);
		menu->setWindowActive(true);
		menu->updateLayout();
	}

	Base::onMouseEvent(ev);
}

void EdUIDiagram::onKeyEvent(axUIKeyEvent& ev) {
	using Type = axUIKeyEvent::Type;
	using KeyCode = axUIKeyCode;

	switch (ev.type) {
		case Type::Down: {
			switch (ev.key) {
				case KeyCode::Delete: {
					_deleteSelectedItems();
					return;
				}
			}
		}break;
	}
}

void EdUIDiagram::_deleteSelectedItems() {
	for (auto& it : Base::selection()) {
		onDeleteDiaItem(&it);
	}
	clearSelection(false);
}

void EdUIDiagram::onDeleteDiaItem(Item* item) {
	if (auto* view = ax_type_cast<EdUIDiaNode>(item)) {
		if (auto* node = view->diaNode()) {
			ax_delete(node);
		}
	}

	if (auto* view = ax_type_cast<EdUIDiaConnection>(item)) {
		if (auto* conn = view->diaConnection()) {
			ax_delete(conn);
		}
	}
}

bool EdUIDiagram::onAcceptConnection(axUIDiaConnection* conn_) {
	auto* conn = ax_type_cast<EdUIDiaConnection>(conn_);
	if (!conn) return false;

	auto* dia = diagram();
	if (!dia) return false;

	auto* inputProp = conn->inputProp();
	if (!inputProp) return false;

	auto* inputNode = inputProp->node();
	if (!inputNode) return false;

	auto* outputProp = conn->outputProp();
	if (!outputProp) return false;

	auto* outputNode = outputProp->node();
	if (!outputNode) return false;

	auto* diaConn = dia->onAcceptConnection(outputNode->name(),	outputProp->propName(),
											inputNode->name(),	inputProp->propName());

	conn->setLinkedObject(diaConn);
	return diaConn;
}

}}
