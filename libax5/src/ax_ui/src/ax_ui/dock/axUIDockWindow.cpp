#include "axUIDockWindow.h"
#include "axUIDockPanel.h"
#include "axUIDockApp.h"

axUIDockWindow::axUIDockWindow(axUIView* parent, CreateDesc& desc) 
	: Base(parent, desc)
	, _headerArea(this)
	, _dockZone(this)
	, _footerArea(this)
{
	setLayoutHandler(&_vboxLayoutHandler);
	_dockZone.layout.setExpandW();
	_dockZone.layout.setExpandH();
}

void axUIDockWindow::onSetNeedToRender() {
	_dockZone.setNeedToRender();
}

axUIDockWindow::DrawableArea::DrawableArea(axUIView* parent)
	: Base(parent, true)
{	
	layout.setFitContentH();
}
