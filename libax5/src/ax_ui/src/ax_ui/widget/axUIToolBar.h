#pragma once

#include "axUIButton.h"
#include "axUIChoiceBox.h"
#include "../layout/axUIBoxLayout.h"

class axUIToolBar : public axUIHBoxView {
	AX_RTTI_CLASS(axUIToolBar, axUIHBoxView)
public:
	axUIToolBar(axUIView* parent);

	virtual void onMouseEvent (axUIMouseEvent& ev) override {}
};
