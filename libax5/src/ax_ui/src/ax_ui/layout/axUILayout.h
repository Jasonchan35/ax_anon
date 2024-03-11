#pragma once

#include "../base/axUIView.h"

class axUILayoutHandler : public axObject {
	AX_RTTI_CLASS(axUILayoutHandler, axObject)
public:
	using Vec2 = axVec2f;
	using Vec3 = axVec3f;
	using Vec4 = axVec4f;
	using Mat4 = axMat4f;
	using Rect2 = axRect2f;
	using Margin2 = axMargin2f;

	virtual void onUpdateLayout	(axUIView* view) = 0;
	virtual void onMeasureLayout(axUIView* view, axUIMeasureRequest& req) = 0;
};

class axUILayoutView : public axUIView {
	AX_RTTI_CLASS(axUILayoutView, axUIView);
public:
	axUILayoutView(axUIView* parent) : Base(parent) {}
};
