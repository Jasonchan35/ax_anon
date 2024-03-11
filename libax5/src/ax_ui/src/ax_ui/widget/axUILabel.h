#pragma once

#include <ax_ui/base/axUIView.h>
#include <ax_ui/text/axUITextMesh.h>

class axUILabel : public axUIView {
	AX_RTTI_CLASS(axUILabel, axUIView)
public:
	axUILabel(axUIView* parent, axStrView text = "");

	virtual void onRender(axRenderRequest& req) override;
	virtual void onMeasureLayout(axUIMeasureRequest& req) override;

	void setText(axStrViewU text);
	void setText(axStrView text) { setText(axTempStringU::s_convert(text)); }
	axStrViewU text() const;

	void setTextAlign(const Align& v) { _mesh.setAlign(v); }

private:
	axStringU		_text;
	axUITextMesh	_mesh;
};
