#include "axUIDockZone.h"
#include "axUIDockPanel.h"
#include "axUIDockApp.h"
#include "axUIDockWindow.h"

const float axUIDockZone::kDividerSize = 8;

axUIDockZone::axUIDockZone(axUIView* parent)
	: Base(parent)
{
}

axUIDockZone* axUIDockZone::split(float size, bool isVertical) {
	if (_splitted) {
		return _zone0->split(size, isVertical);
	}

	_splitted = true;
	_zone0.ref(ax_new axUIDockZone(this));
	_zone1.ref(ax_new axUIDockZone(this));
	_divider.ref(ax_new Divider(this));

	_splittedSize = size;
	_splitIsVertical = isVertical;

	// hand over panelArea
	if (_panelArea) {
		_panelArea->setParent(_zone0.ptr());
		_zone0->_panelArea = ax_move(_panelArea);
	}

	setNeedToMeasure();
	updateLayout();
	return _zone1.ptr();
}

void axUIDockZone::addPanel(axUIDockPanel* panel) {
	if (_zone0) {
		_zone0->addPanel(panel);
		return;
	}

	if (!_panelArea) {
		_panelArea.ref(ax_new PanelArea(this));
	}
	_panelArea->addPanel(panel);
	setNeedToLayout();
}

axUIDockPanel* axUIDockZone::getActivePanel() {
	return _panelArea ? _panelArea->getActivePanel() : nullptr;
}

void axUIDockZone::onPanelTitleChanged(axUIDockPanel* panel) {
	if (_panelArea) {
		_panelArea->onPanelTitleChanged(panel);
	}
}

void axUIDockZone::onSetNeedToRender() {
	if (_panelArea) _panelArea->setNeedToRender();
	if (_zone0) _zone0->setNeedToRender();
	if (_zone1) _zone1->setNeedToRender();
}

void axUIDockZone::onUpdateLayout() {
	auto rc = contentRect();

	if (_splitted) {
		if (_splitIsVertical) {
			auto s = ax_clamp(rc.h - _splittedSize, 0.0f, rc.h);
			_zone0->setLayoutResultRect(rc.trimTop(s));
			_zone1->setLayoutResultRect(rc);
			_divider->setLayoutResultRect(Rect2(0, s - kDividerSize / 2, rc.w, kDividerSize));

		}else{
			auto s = ax_clamp(rc.w - _splittedSize, 0.0f, rc.w);
			_zone0->setLayoutResultRect(rc.trimLeft(s));
			_zone1->setLayoutResultRect(rc);
			_divider->setLayoutResultRect(Rect2(s - kDividerSize / 2, 0, kDividerSize, rc.h));
		}

		_zone0->setNeedToLayout();
		_zone1->setNeedToLayout();
	} else {
		if (_panelArea) {
			_panelArea->setLayoutResultRect(rc);
		}
	}
}

void axUIDockZone::onMeasureLayout(axUIMeasureRequest& req) {
	// do nothing, because it does not depends on children
}

#if 0
#pragma mark --------------- Divider ------------------------------
#endif

const float axUIDockZone::Divider::kDividerSize = axUIDockZone::kDividerSize;

axUIDockZone::Divider::Divider(axUIDockZone* parent) 
	: Base(parent, false)
	, _zone(parent)
{

}

void axUIDockZone::Divider::setCursorResize() {
	auto cursorType = _zone->splitIsVertical() ? axUICursorType::ResizeNS : axUICursorType::ResizeWE;
	axUIApp::s_instance()->setCursor(cursorType);
}

void axUIDockZone::Divider::setCursorArrow() {
	axUIApp::s_instance()->setCursor(axUICursorType::Arrow);
}

void axUIDockZone::Divider::onMouseEvent(axUIMouseEvent& ev) {
	switch (ev.type) {
		case axUIMouseEventType::Enter: {
			setCursorResize();
		} break;

		case axUIMouseEventType::Move: {
			setCursorResize();
		} break;

		case axUIMouseEventType::Leave: {
			setCursorArrow();
		} break;

		case axUIMouseEventType::DragBegin: {
			setCursorResize();
			if (ev.button == axUIMouseEvent::Button::Left) {
				_startSplittedSize = _zone->splittedSize();
				_dragging = true;

				CreateDesc desc;
				desc.winDesc.alwaysOnTop = true;
				createRenderContext(desc);
			}
		} break;

		case axUIMouseEventType::Dragging: {
			setCursorResize();
			if (ev.pressedButtons == axUIMouseEvent::Button::Left) {
				auto zoneSize = _zone->size();
				auto s = _startSplittedSize;

				if (_zone->splitIsVertical()) {
					s = ax_round(s - ev.dragDeltaPos.y);
					ax_clamp_it(s, kDividerSize, zoneSize.y - kDividerSize);
					_zone->setSplittedSize(s);
					setPos(0, zoneSize.y - s);
				} else {
					s = ax_round(s - ev.dragDeltaPos.x);
					ax_clamp_it(s, kDividerSize, zoneSize.x - kDividerSize);
					_zone->setSplittedSize(s);
					setPos(zoneSize.x - s, 0);
				}
			}

			_zone->setNeedToLayout();
		} break;

		case axUIMouseEventType::DragEnd: {
			_zone->setNeedToLayout();
			_dragging = false;
			destroyRenderContext();
			setCursorArrow();
		} break;	
	}
}

void axUIDockZone::Divider::onRenderContextRender(axRenderRequest& req) {
	req.clearColorAndDepthBuffers(axColor(.5f, .5f, .5f), 0);
}

#if 0
#pragma mark --------------- TabBar ------------------------------
#endif

axUIDockZone::TabBar::TabBar(axUIView* parent) 
	: Base(parent)
{
	setBgColor(.1f, .1f, .1f);
	layout.setFitContentH();
}

void axUIDockZone::TabBar::onPanelTitleChanged(axUIDockPanel* panel) {
	for (auto& c : children()) {
		if (auto* tb = ax_type_cast<TabButton>(&c)) {
			if (tb->_panel == panel) {
				tb->label.setText(panel->title());
				setNeedToRender();
			}
		}
	}
}

#if 0
#pragma mark --------------- PanelArea ------------------------------
#endif

axUIDockZone::PanelArea::PanelArea(axUIDockZone* parent)
	: Base(parent, true)
	, _tabBar(this)
{
}

void axUIDockZone::PanelArea::onRenderContextRender(axRenderRequest& req) {
	auto rc = contentRect();
	req.setViewport(rc);

	axStrView name = "PanelArea";
	if (_activeTab) {
		name = _activeTab->debugName();
	}

	AX_ZoneScoped;
	AX_ZoneText(name.data(), name.size());

	render(req);
}

void axUIDockZone::PanelArea::onRenderChildren(axRenderRequest& req) {
	auto* activePanel = getActivePanel();

	auto rc = contentRect();
	auto bgColor = activePanel ? activePanel->bgColor() : axColor(.2f, .2f, .2f);
	req.clearColorAndDepthBuffers(bgColor, 1);


	if (activePanel) {
		auto panelRect = activePanel->worldRect();
		auto pos = panelRect.pos - worldPos();
	
		auto oldViewport = req.viewport();
		req.setViewport(Rect2(pos, panelRect.size));

		auto mv = req.ui.modelview.push();
		mv *= Mat4::s_translate(-activePanel->rect().pos);

		auto proj = req.ui.projection.push();
		req.ui.setProjectMatrix(panelRect.size);

		activePanel->render(req);

		req.setViewport(oldViewport);
	}

	_tabBar.render(req);
	req.ui.drawRectBorder(AX_LOC, rc, axMargin2f(1,0,0,1), axColor(0,0,0));
}

void axUIDockZone::PanelArea::onMeasureLayout(axUIMeasureRequest& req) {
	// do nothing, because it does not depends on children
}

void axUIDockZone::PanelArea::onUpdateLayout() {
	auto rc = contentRect();

	_tabBar.setLayoutResultRect(rc);
	rc.trimTop(_tabBar.height());

	if (auto* p = getActivePanel()) {
		p->setLayoutResultRect(rc);
	}
}

void axUIDockZone::PanelArea::onPanelTitleChanged(axUIDockPanel* panel) {
	_tabBar.onPanelTitleChanged(panel);
}

axUIDockPanel* axUIDockZone::PanelArea::getActivePanel() {
	if (!_activeTab) {
		_activeTab = _panels.head();
	}
	return _activeTab;
}

void axUIDockZone::PanelArea::addPanel(axUIDockPanel* panel) {
	_panels.forceAppend(panel);
	getActivePanel();
	panel->setParent(this);

	auto tabButton = ax_new TabButton(&_tabBar, panel->title());
	tabButton->_panel = panel;

	setNeedToMeasure();
}

axUIDockZone* axUIDockZone::PanelArea::getZone() {
	return ax_type_cast<axUIDockZone>(parent());
}

axUIDockZone* axUIDockZone::PanelArea::getZoneParent() {
	auto* zone = getZone();
	return ax_type_cast<axUIDockZone>(zone->parent());

}

bool axUIDockZone::PanelArea::isZone1() {
	if (auto* p = getZoneParent()) {
		return p->_zone1 == getZone();
	}
	return false;
}
