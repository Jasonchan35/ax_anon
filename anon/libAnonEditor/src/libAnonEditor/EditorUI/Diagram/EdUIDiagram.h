#pragma once

namespace Anon { namespace Editor {

class EdUIDiaNode;
class DiagramPanel;

class EdUIDiagram : public axUIDiagram {
	AX_RTTI_CLASS(EdUIDiagram, axUIDiagram);
public:
	EdUIDiagram(DiagramPanel* parent);

	DiagramPanel* panel() { return _panel; }

	void setDiagram(Diagram* dia);
	Diagram* diagram() { return _diagram; }

	EdUIDiaNode*	getViewFromNode(AnonObject* node);

	void onEditorSelectionChanged();

protected:
	virtual bool onWillAcceptConnection	(axUIDiaProp* src, axUIDiaProp* dst) override;
	virtual bool onAcceptConnection		(axUIDiaConnection* conn) override;

	virtual void onMouseEvent		(axUIMouseEvent& ev) override;
	virtual void onKeyEvent			(axUIKeyEvent& ev) override;
	virtual void onDeleteDiaItem	(Item* item);

	virtual void onReloadContent() override;
	void reload_node (axUIDiaNode*   view, DiaNode*   node);
	void reload_group(axUIDiaGroup*  view, DiaGroup*  group);
	void reload_connections(DiaNode* node);

	void _createNode(axStrView typeName);

	virtual axUIDiaConnection* onNewConnectionView(axUIDiaSocket* parent, axUIDiaSocket* target) override;

private:
	void _deleteSelectedItems();

	DiagramPanel*		_panel = nullptr;
	axLink<Diagram>		_diagram;
};


}}
