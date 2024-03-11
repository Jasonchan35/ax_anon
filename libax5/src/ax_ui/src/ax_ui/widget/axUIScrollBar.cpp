#include "axUIScrollBar.h"

axUIScrollBar::axUIScrollBar(axUIView* parent, bool isVertical)
	: Base(parent)
	, _thumbView(this)
	, _isVertical(isVertical)
	, _bgColor(.5f, .5f, .5f)
{

}

void axUIScrollBar::onRender(axRenderRequest& req) {
}

void axUIScrollBar::onUpdateLayout() {
	auto rc = contentRect();
	if (_isVertical) {
		auto y = ax_ceil(rc.y + rc.h * _thumbStart);
		auto h = ax_floor(rc.h * _thumbEnd - y);
		_thumbView.setLayoutResultRect(Rect2(rc.x, y, rc.w, h));
	} else {
		auto x = ax_ceil(rc.x + rc.w * _thumbStart);
		auto w = ax_floor(rc.w * _thumbEnd - x);
		_thumbView.setLayoutResultRect(Rect2(x, rc.y, w, rc.h));
	}
}

void axUIScrollBar::setThumb(float start, float end) {
	_thumbStart = ax_clamp01(start);
	_thumbEnd   = ax_clamp01(end);
	updateLayout();
}

axUIScrollBar::ThumbView::ThumbView(axUIScrollBar* parent)
	: Base(parent)
	, _color(.3f, .3f, .3f)
	, _scrollBar(parent)
{

}

void axUIScrollBar::ThumbView::onRender(axRenderRequest& req) {
}

void axUIScrollBar::ThumbView::onMouseEvent(axUIMouseEvent& ev) {
	using Type   = axUIMouseEvent::Type;
	using Button = axUIMouseEvent::Button;
	switch (ev.type)
	{
		case Type::Down: {
			if (ev.button == Button::Left) {
				DragEvent outEvent;
				outEvent.type = DragEvent::Type::Start;
				_scrollBar->evDragging.emit(_scrollBar, outEvent);
			}
		}break;

		case Type::Dragging: {
			if (ev.pressedButtons == Button::Left) { //only left button down
				DragEvent outEvent;
				outEvent.type = DragEvent::Type::Dragging;
				outEvent.dragDeltaPos = ev.dragDeltaPos;

				if (_scrollBar->isVertical()) {
					outEvent.dragDeltaPos.y = ev.dragDeltaPos.y;
				} else {
					outEvent.dragDeltaPos.x = ev.dragDeltaPos.x;
				}
				_scrollBar->evDragging.emit(_scrollBar, outEvent);
			}
		}break;
	}
}
