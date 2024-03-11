#include "axUIFlowLayout.h"

axUIFlowLayoutHandler::axUIFlowLayoutHandler(bool isVertical, float spacing)
	: _isVertical(isVertical)
	, _spacing(spacing)
{
}

void axUIFlowLayoutHandler::onUpdateLayout(axUIView* view) {
	auto rect = view->contentRect();

	axVec2f offset(0,0);
	axInt i = 0;
	for (auto& c : view->children()) {
		axRect2f childRect;
		auto& s = c.layout.measured();
		childRect.size = ax_max(c.size(), s.min);

		if (_isVertical) {
			if (i > 0) offset.y += _spacing;
			childRect.pos = offset;

			//new column
			if (childRect.yMax() > rect.h) {
				childRect.y = 0;
				offset.y = 0;
				i = 0;
			}

			offset.y += c.height();
		} else {
			if (i > 0) offset.x += _spacing;
			childRect.pos = offset;

			//new row
			if (childRect.xMax() > rect.w) {
				childRect.x = 0;
				offset.x = 0;
				i = 0;
			}

			offset.x += c.width();
		}

		c.setLayoutResultRect(childRect);
		i++;
	}
}

void axUIFlowLayoutHandler::onMeasureLayout(axUIView* view, axUIMeasureRequest& req) {
	axInt i = 0;
	for (auto& c : view->children()) {
		auto& cm = c.layout.measured();

		ax_max_it(req.result.min, cm.min);
		if (i > 0) {
			req.result.max += _spacing;
		}
		req.result.max += cm.max;

		i++;
	}
}