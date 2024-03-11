#pragma once

#include "../base/axUIView.h"

class axUIDockZone;
class axUIDockPanel : public axUIView, public axDListNode<axUIDockPanel> {
	AX_RTTI_CLASS(axUIDockPanel, axUIView)
public:
	using PanelListNode = axDListNode<axUIDockPanel>;

	axUIDockPanel(axUIDockZone* zone);

	void setTitle(axStrView s);
	axStrView title() { return _title; }

protected:
	virtual void onMeasureLayout(axUIMeasureRequest& req) override;

private:
	axString		_title;
};
