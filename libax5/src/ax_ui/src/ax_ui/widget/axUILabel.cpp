#include "axUILabel.h"
#include "../base/axUIDefaultAssets.h"

axUILabel::axUILabel(axUIView* parent, axStrView text) 
	: Base(parent)
{
	_mesh.setColor(axUIDefaultAssets::s_instance()->fontStyles.std->color);
	_mesh.setIsInlineDraw(true);
	layout.setFitContentW();
	layout.setFitContentH();
	setPadding({4, 2});
	setText(text);
}

void axUILabel::onRender(axRenderRequest& req) {
	_mesh.renderInRect(req, contentRect());
}

void axUILabel::onMeasureLayout(axUIMeasureRequest& req) {
	_mesh.onMeasureLayout(req);
}

void axUILabel::setText(axStrViewU text) {
	_text = text;
	_mesh.setText(text);
	setNeedToMeasure();
}

axStrViewU axUILabel::text() const {
	return _text;
}

