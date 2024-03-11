#include "DiagramPanel.h"
#include "../../EditorUI/EdUICommon.h"
#include "../../App/EditorApp.h"

namespace Anon{ namespace Editor {

DiagramPanel::DiagramPanel(axUIDockZone* zone) 
	: Base(zone)
	, _diagram(this)
{
	setTitle("Node Diagram");
	EditorApp::s_instance()->evOpenDiagramPanel.bind(this, &This::_openDiagramPanel);
}

DiagramPanel::~DiagramPanel() {
	setDiagram(nullptr);
}

void DiagramPanel::setDiagram(Diagram* dia) {
	_diagram.setDiagram(dia);
}

void DiagramPanel::onMouseEvent(axUIMouseEvent& ev) {
	using Type = axUIMouseEvent::Type;
	if (ev.type == Type::Down) {
		setNeedToRender();
	}
}

void DiagramPanel::onEditorSelectionChanged() {
	_diagram.onEditorSelectionChanged();
}

void DiagramPanel::onAnonContextChanged(AnonContext::ChangedFlags flags) {
	_diagram.reloadContent();
}

void DiagramPanel::onRender(axRenderRequest& req) {
	Base::onRender(req);
//	req.drawStatistics(Vec2f(width() - 360, 40));
}

void DiagramPanel::_openDiagramPanel(Diagram* dia) {
	setDiagram(dia);
}

}} //namespace
