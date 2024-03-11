#pragma once

#include "axUILayout.h"

class axUIBoxLayoutHandler : public axUILayoutHandler {
	AX_RTTI_CLASS(axUIBoxLayoutHandler, axUILayoutHandler)
public:
	axUIBoxLayoutHandler(bool isVertical = false, float spacing = 0);

	virtual void onUpdateLayout	(axUIView* view) override;
	virtual void onMeasureLayout(axUIView* view, axUIMeasureRequest& req) override;

	This& setIsVertical(bool b) &	{ _isVertical = b; return *this; }
	bool  isVertical() const		{ return _isVertical; }

	This& setSpacing(float v) &		{ _spacing = v; return *this; }
	float spacing() const			{ return _spacing; }

private:
	bool	_isVertical;
	float	_spacing;
};

class axUIHBoxLayoutHandler : public axUIBoxLayoutHandler {
public:
	axUIHBoxLayoutHandler(float spacing = 0) : axUIBoxLayoutHandler(false, spacing) {}
};

class axUIVBoxLayoutHandler : public axUIBoxLayoutHandler {
public:
	axUIVBoxLayoutHandler(float spacing = 0) : axUIBoxLayoutHandler(true, spacing) {}
};


// Wrap axUILayoutHandler into view
template<class T>
class axUIBoxViewT : public axUILayoutView {
	using Base = axUILayoutView;
public:
	axUIBoxViewT(axUIView* parent) : Base(parent) { setLayoutHandler(&boxLayout); }
	T boxLayout;
};

class axUIBoxView     : public axUIBoxViewT<axUIBoxLayoutHandler> {
	AX_RTTI_CLASS(axUIBoxView, axUIBoxViewT<axUIBoxLayoutHandler>)
public:
	axUIBoxView(axUIView* parent) : Base(parent) {}
};

class axUIHBoxView     : public axUIBoxViewT<axUIHBoxLayoutHandler> {
	AX_RTTI_CLASS(axUIHBoxView, axUIBoxViewT<axUIHBoxLayoutHandler>)
public:
	axUIHBoxView(axUIView* parent) : Base(parent) {}
};

class axUIVBoxView     : public axUIBoxViewT<axUIVBoxLayoutHandler> {
	AX_RTTI_CLASS(axUIVBoxView, axUIBoxViewT<axUIVBoxLayoutHandler>)
public:
	axUIVBoxView(axUIView* parent) : Base(parent) {}
};
