#pragma once

#include "axUILayout.h"

class axUIFlowLayoutHandler : public axUILayoutHandler {
	AX_RTTI_CLASS(axUIFlowLayoutHandler, axUILayoutHandler)
public:
	axUIFlowLayoutHandler(bool isVertical = false, float spacing = 0);

	void setIsVertical(bool b)	{ _isVertical = b; }
	bool isVertical() const		{ return _isVertical; }

	void setSpacing(float v)	{ _spacing = v; }
	float spacing() const		{ return _spacing; }

	virtual	void onUpdateLayout	(axUIView* view) override;
	virtual void onMeasureLayout(axUIView* view, axUIMeasureRequest& req) override;

private:
	bool	_isVertical = false;
	float	_spacing;
};


class axUIFlowLayoutView : public axUILayoutView {
	AX_RTTI_CLASS(axUIFlowLayoutView, axUILayoutView);
public:
	axUIFlowLayoutView(axUIView* parent) : Base(parent) { setLayoutHandler(&flowLayout); }
	axUIFlowLayoutHandler flowLayout;
};

