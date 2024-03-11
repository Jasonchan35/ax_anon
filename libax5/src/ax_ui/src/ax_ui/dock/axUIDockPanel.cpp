#include "axUIDockPanel.h"
#include "axUIDockWindow.h"
#include "axUIDockZone.h"

axUIDockPanel::axUIDockPanel(axUIDockZone* zone) 
	: Base(zone)
{
	setBgColor(.2f, .2f, .2f);
	if (!zone) {
		throw axError_Undefined(AX_LOC);
	}
	zone->addPanel(this);
}

void axUIDockPanel::setTitle(axStrView s) {
	_title = s;

	if (auto* a = ax_type_cast<axUIDockZone::PanelArea>(_parent)) {
		a->onPanelTitleChanged(this);
	} else {
		AX_ASSERT(false);
	}
}

void axUIDockPanel::onMeasureLayout(axUIMeasureRequest& req) {
	// do nothing, because it does not depends on children
}
