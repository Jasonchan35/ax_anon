#pragma once

#include "../base/axUIView.h"

class axUIScrollBar : public axUIView {
	AX_RTTI_CLASS(axUIScrollBar, axUIView)
public:
	axUIScrollBar(axUIView* parent, bool isVertical);

	virtual void onRender(axRenderRequest& req) override;
	virtual void onUpdateLayout() override;

	bool isVertical		() const { return _isVertical; }

	void  setThumb		(float start, float end);
	float thumbStart	() const { return _thumbStart; }
	float thumbEnd		() const { return _thumbEnd;   }
	bool  thumbIsFull	() const { return _thumbStart <= 0 && _thumbEnd >= 1; }

	struct DragEvent {
		enum class Type {
			Unknown,
			Start,
			Dragging,
			End,
		};
		Type	type = Type::Unknown;
		Vec2	dragDeltaPos {0,0};
	};

	axDelegate<void (axUIScrollBar* sender, DragEvent& ev)> evDragging;

private:
	class ThumbView : public axUIView {
		AX_RTTI_CLASS(ThumbView, axUIView)
	public:
		ThumbView(axUIScrollBar* parent);

		virtual void onRender(axRenderRequest& req) override;
		virtual void onMouseEvent(axUIMouseEvent& ev) override;
		axColor _color;
		axUIScrollBar* _scrollBar;
	};

	ThumbView _thumbView;

	bool	_isVertical   = false;
	float	_thumbStart = 0;
	float	_thumbEnd   = 1;
	axColor	_bgColor;
};
