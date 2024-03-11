#include "ConsolePanel.h"

namespace Anon{ namespace Editor {

ConsolePanel::ConsolePanel(axUIDockZone* zone) 
	: Base(zone)
{
	setTitle("Console");
}

void ConsolePanel::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.type == axUIMouseEvent::Type::Down) {
		setNeedToRender();
	}
}

}} //namespace
