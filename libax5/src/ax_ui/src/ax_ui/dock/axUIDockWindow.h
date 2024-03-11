#pragma once

#include <ax_ui/base/axUIWindow.h>
#include <ax_ui/base/axUIRenderContextView.h>
#include <ax_ui/layout/axUIBoxLayout.h>
#include <ax_ui/layout/axUIFlowLayout.h>
#include <ax_ui/widget/axUIToolBar.h>
#include <ax_ui/dock/axUIDockZone.h>

class axUIDockPanel;


class axUIDockWindow : public axUIWindow {
	AX_RTTI_CLASS(axUIDockWindow, axUIWindow)
public:
	class DrawableArea : public axUIRenderContextView {
		AX_RTTI_CLASS(DrawableArea, axUIRenderContextView)
	public:
		DrawableArea(axUIView* parent);
	};

	axUIDockWindow(axUIView* parent, CreateDesc& desc);
	DrawableArea* headerArea()	{ return &_headerArea; }
	axUIDockZone* dockZone()	{ return &_dockZone; }
	DrawableArea* footerArea()	{ return &_footerArea; }

protected:
	virtual void onSetNeedToRender() override;

private:
	axUIVBoxLayoutHandler		_vboxLayoutHandler;

	DrawableArea	_headerArea;
	axUIDockZone	_dockZone;
	DrawableArea	_footerArea;
};
