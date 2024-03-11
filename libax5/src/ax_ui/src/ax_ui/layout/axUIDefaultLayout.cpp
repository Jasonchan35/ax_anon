#include "axUIDefaultLayout.h"

axUIDefaultLayoutHandler* axUIDefaultLayoutHandler::s_instance() {
	static axUIDefaultLayoutHandler s;
	return &s;
}

void axUIDefaultLayoutHandler::onUpdateLayout(axUIView* view) {
	Rect2 rc = view->contentRect();
	for (auto& c : view->children()) {
		if (!c.visible()) continue;
		if (c.layout.excludeFromLayout()) continue;
		c.setLayoutResultRect(rc);
	}	
}

void axUIDefaultLayoutHandler::onMeasureLayout(axUIView* view, axUIMeasureRequest& req) {
	for (auto& c : view->children()) {
		if (!c.visible()) continue;
		if (c.layout.excludeFromLayout()) continue;

		auto& cmr = c.layout.measured();
		ax_max_it(req.result.min, cmr.min);
	}
}
