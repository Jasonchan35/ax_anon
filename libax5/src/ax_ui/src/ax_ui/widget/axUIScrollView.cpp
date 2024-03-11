#include "axUIScrollView.h"

axUIScrollView::ContentView::ContentView(axUIView* parent) 
	: Base(parent) 
{
}

axUIScrollView::axUIScrollView(axUIView* parent)
	: Base(parent)
	, _scrollBarCornerColor(.3f, .3f, .3f)
	, _content(this)
	, _scrollBarH(this, false)
	, _scrollBarV(this, true )
	, _showScrollBarH(true)
	, _showScrollBarV(true)
	, _autoHideScrollBarH(true)
	, _autoHideScrollBarV(true)
{
	_scrollBarV.evDragging.bind(this, &This::onScrollBarDragging);
	setWantMouseWheelEvent(true);
}

void axUIScrollView::onRender(axRenderRequest& req) {
	_content.render(req);
	_scrollBarH.render(req);
	_scrollBarV.render(req);

	if (_scrollBarH.visible() && _scrollBarV.visible()) {
		axVec2f s(_scrollBarSize);

		req.ui.drawRect(AX_LOC, axRect2f(size() - s, s), _scrollBarCornerColor);
	}
}

void axUIScrollView::onUpdateLayout() {
	{
		auto rc = _content.rect();
		auto& mr = _content.layout.measured();
		rc.size = ax_max(mr.min, contentRect().size);
		_content.setLayoutResultRect(rc);
	}

	setContentPos(contentPos());
	updateScrollBars();
}

void axUIScrollView::onMouseEvent(axUIMouseEvent& ev) {
	switch (ev.type) {
		case axUIMouseEvent::Type::Wheel: {
			setContentPos(_content.pos() + ev.wheelDelta);
		}break;
	}
}

void axUIScrollView::updateScrollBars() {
	auto rc = localRect();

	bool showH = true;
	bool showV = true;

	{
		auto s = _content.rect();
		_scrollBarV.setThumb(-s.y / s.h, (rc.h - s.y ) / s.h);
		_scrollBarH.setThumb(-s.x / s.w, (rc.w - s.x ) / s.w);

		if (!_showScrollBarH) {
			showH = false;
		} else if (_autoHideScrollBarH && _scrollBarH.thumbIsFull()) {
			showH = false;
		}

		if (!_showScrollBarV) {
			showV = false;
		} else if (_autoHideScrollBarV && _scrollBarV.thumbIsFull()) {
			showV = false;
		}
	}

	_scrollBarH.setVisible(showH);
	_scrollBarV.setVisible(showV);

	float subY = _scrollBarSize;
	float subX = _scrollBarSize;

	if (!showH) subY = 0;
	if (!showV)	subX = 0;

	_scrollBarH.setLayoutResultRect(Rect2(0, rc.h - subY, rc.w - subX, _scrollBarSize));
	_scrollBarV.setLayoutResultRect(Rect2(rc.w - subX, 0, _scrollBarSize, rc.h - subY));
}

void axUIScrollView::setScrollableContent(axUIView* v) {
	v->setParent(&_content);
}

axUIView* axUIScrollView::scrollableContent() {
	return &_content;
}

void axUIScrollView::setShowScrollBar(bool horizontal, bool vertical) {
	_showScrollBarH = horizontal;
	_showScrollBarV = vertical;
}

void axUIScrollView::setAutoHideScrollBar(bool horizontal, bool vertical) {
	_autoHideScrollBarH = horizontal;
	_autoHideScrollBarV = vertical;
}

void axUIScrollView::setContentPos(const axVec2f& newPos_) {
	auto newPos = newPos_;

	auto remain = ax_min(size() - _content.size(), Vec2(0,0));

	newPos = ax_clamp(newPos, remain, Vec2(0,0));
	newPos = ax_round(newPos);

	_content.setPos(newPos);

	updateScrollBars();
	setNeedToRender();
}

axUIView::Vec2 axUIScrollView::contentPos() const {
	return _content.pos();
}

void axUIScrollView::onScrollBarDragging(axUIScrollBar* sender, axUIScrollBar::DragEvent& ev) {
	using Type = axUIScrollBar::DragEvent::Type;

	if (ev.type == Type::Start) {
		_startDragContentPos = _content.pos();

	} else if (ev.type == Type::Dragging) {
		if (size().x == 0 || size().y == 0)
			return;

		auto pos = _startDragContentPos;
		auto delta = ev.dragDeltaPos * _content.size() / size();

		pos -= delta;

		setContentPos(pos);
	}
}
