#pragma once

#include "../base/axUIView.h"

class axUISimpleDraw : public axUIView {
	AX_RTTI_CLASS(axUISimpleDraw, axUIView);
public:

	virtual void onRender(axRenderRequest& req) override;

	axColor		color;
	axColor		borderColor;
	axMargin2f	borderWidth;
//	axUITexture	texture;
};