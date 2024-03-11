#pragma once

#include "../../EditorUI/Diagram/EdUIDiagram.h"
#include "../EditorPanel.h"

namespace Anon{ namespace Editor {

class DiagramPanel : public EditorPanel {
	AX_RTTI_CLASS(DiagramPanel, EditorPanel)
public:
	DiagramPanel(axUIDockZone* zone);
	virtual ~DiagramPanel() override;

	void setDiagram(Diagram* dia);

	void onMouseEvent(axUIMouseEvent& ev) override;
	void onEditorSelectionChanged() override;
	void onAnonContextChanged(AnonContext::ChangedFlags flags) override;
	void onRender(axRenderRequest& req) override;

	EdUIDiagram*	diagram() { return &_diagram; }

private:
	void _openDiagramPanel(Diagram* dia);

	EdUIDiagram	_diagram;
};

}} //namespace
