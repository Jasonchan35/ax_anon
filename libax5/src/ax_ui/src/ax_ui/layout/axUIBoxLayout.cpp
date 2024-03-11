#include "axUIBoxLayout.h"

axUIBoxLayoutHandler::axUIBoxLayoutHandler(bool isVertical, float spacing)
	: _isVertical(isVertical)
	, _spacing(spacing)
{
}

void axUIBoxLayoutHandler::onUpdateLayout(axUIView* view) {
	axInt childCount = view->children().size();
	if (!childCount) return;

	auto contentRect = view->contentRect();

	float remainSize = _isVertical ? contentRect.size.y : contentRect.size.x;
	float totalProportion(0);

	using M = axUILayoutMode;

	// reset temp size, compute totalProportion
	{
		axInt i = 0;
		for (auto& c : view->children()) {
			if (!c.visible()) continue;
			if (c.layout.excludeFromLayout()) continue;

			auto& t  = c.layout._tempSize;
			if (i > 0) remainSize -= _spacing;
			auto& ep = c.layout.expandProportion();
			auto& mm = c.layout.measured().min;

			if (!_isVertical) {
				switch (c.layout.modeW()) {
					case M::Fixed:		t.x = c.rect().w; break;
					case M::FitContent: t.x = mm.x; break;
					case M::Expand:		t.x = 0; totalProportion += ep.x; break;
					default: throw axError_Undefined(AX_LOC);
				}
				remainSize -= t.x;
				t.y = contentRect.h * ax_clamp01(ep.y);
			} else {
				switch (c.layout.modeH()) {
					case M::Fixed:		t.y = c.rect().h; break;
					case M::FitContent: t.y = mm.y; break;
					case M::Expand:		t.y = 0; totalProportion += ep.y; break;
					default: throw axError_Undefined(AX_LOC);
				}
				remainSize -= t.y;
				t.x = contentRect.w * ax_clamp01(ep.x);
			}
		}
		i++;
	}

	// distribute remind size by child proportion
	const auto maxIteration = childCount * 2;
	for (axInt iteration = 0; iteration < maxIteration; iteration++) {
		if (totalProportion <= 0) break;
		if (remainSize <= 0) break;

		float consumed = 0;
		axInt i = 0;
		for (auto& c : view->children()) {
			if (!c.visible()) continue;
			if (c.layout.excludeFromLayout()) continue;

			auto cm = c.layout.measured();
			auto proportion = c.layout.expandProportion();

			float delta = 0;
			if (_isVertical) {
				if (!c.layout.isExpandH()) continue;

				auto oldSize = c.layout._tempSize.y;
				if (oldSize >= cm.max.y) continue;

				delta = remainSize * proportion.y / totalProportion;
				delta = (i == 0) ? ax_ceil(delta) : ax_floor(delta);
			
				auto newSize = ax_clamp(oldSize + delta, cm.min.y, cm.max.y);
				delta = newSize - oldSize;

				c.layout._tempSize.y = newSize;
				if (newSize >= cm.max.y) {
					totalProportion -= proportion.y;
				}

			} else {
				if (!c.layout.isExpandW()) continue;

				auto oldSize = c.layout._tempSize.x;
				if (oldSize >= cm.max.x) continue;

				delta = remainSize * proportion.x / totalProportion;
				delta = (i == 0) ? ax_ceil(delta) : ax_floor(delta);
			
				auto newSize = ax_clamp(oldSize + delta, cm.min.x, cm.max.x);
				delta = newSize - oldSize;

				c.layout._tempSize.x = newSize;
				if (newSize >= cm.max.x) {
					totalProportion -= proportion.x;
				}
			}

			consumed += delta;
			i++;
		}

		remainSize -= consumed;
	}

	// final result
	{
		auto rect = contentRect;
		auto offset = rect.pos;

		if (_isVertical) {
			axInt i = 0;
			for (auto& c : view->children()) {
				if (!c.visible()) continue;
				if (c.layout.excludeFromLayout()) continue;
				if (i > 0) offset.y += _spacing;

				auto& t = c.layout._tempSize;
				switch (c.layout.align().x) {
					case axUIAlignX::Left:		offset.x =  rect.x; break;
					case axUIAlignX::Center:	offset.x = (rect.w - t.x) / 2; break;
					case axUIAlignX::Right:		offset.x =  rect.w - t.x; break;
				}

				Rect2 childRect(offset, t);
				c.setLayoutResultRect(childRect);

				offset.y += c.height();
				i++;
			}
		}else{
			axInt i = 0;
			for (auto& c : view->children()) {
				if (!c.visible()) continue;
				if (c.layout.excludeFromLayout()) continue;
				if (i > 0) offset.x += _spacing;

				auto& t = c.layout._tempSize;
				switch (c.layout.align().y) {
					case axUIAlignY::Top:		offset.y =  rect.y; break;
					case axUIAlignY::Center:	offset.y = (rect.h - t.y) / 2; break;
					case axUIAlignY::Bottom:	offset.y =  rect.h - t.y; break;
				}

				Rect2 childRect(offset, t);
				c.setLayoutResultRect(childRect);

				offset.x += c.width();
				i++;
			}
		}
	}
}

void axUIBoxLayoutHandler::onMeasureLayout(axUIView* view, axUIMeasureRequest& req) {
	axInt i = 0;
	for (auto& c : view->children()) {
		if (!c.visible()) continue;
		if (c.layout.excludeFromLayout()) continue;

		auto& cm = c.layout.measured();

		if (_isVertical) {
			if (i > 0) {
				req.result.min.y += _spacing;
			}
			req.result.min.y += cm.min.y;
			ax_max_it(req.result.min.x, cm.min.x);
		} else {
			if (i > 0) {
				req.result.min.x += _spacing;
			}
			req.result.min.x += cm.min.x;
			ax_max_it(req.result.min.y, cm.min.y);
		}
		i++;
	}
}

