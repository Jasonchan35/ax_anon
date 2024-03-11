#pragma once

#include "axUIScrollBar.h"
#include "../layout/axUIBoxLayout.h"

class axUIScrollView : public axUIView {
	AX_RTTI_CLASS(axUIScrollView, axUIView)
public:
	axUIScrollView(axUIView* parent);

	virtual void onRender(axRenderRequest& req) override;
	virtual void onUpdateLayout() override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

	void setScrollableContent(axUIView* v);
	axUIView* scrollableContent();

	void setShowScrollBar		(bool horizontal, bool vertical);
	void setAutoHideScrollBar	(bool horizontal, bool vertical);

	void setContentPos(const axVec2f& newPos);
	Vec2 contentPos() const;

	void offsetContentPos(const axVec2f& deltaPos) { setContentPos(contentPos() + deltaPos); }

	void updateScrollBars();
private:
	void onScrollBarDragging(axUIScrollBar* sender, axUIScrollBar::DragEvent& ev);

	float _scrollBarSize = 20;

	axColor	_scrollBarCornerColor;

	class ContentView : public axUIView {
		AX_RTTI_CLASS(ContentView, axUIView);
	public:
		ContentView(axUIView* parent);
	};

	ContentView		_content;

	axUIScrollBar	_scrollBarH;
	axUIScrollBar	_scrollBarV;

	Vec2			_startDragContentPos;

	bool _showScrollBarH : 1;
	bool _showScrollBarV : 1;
	bool _autoHideScrollBarH : 1;
	bool _autoHideScrollBarV : 1;
};