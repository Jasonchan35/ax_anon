#include "axUIContextMenu.h"

axUIContextMenu::CreateDesc::CreateDesc() {
	type = Type::PopupWindow;
	alwaysOnTop = true;
}

axUIContextMenu::axUIContextMenu(axUIView* parent) 
	: Base(parent, _createDesc())
	, _renderView(this, false)
	, _menu(&_renderView)
{
	auto* mgr = axUIInputManager::s_instance();
	mgr->setMouseCapture(nullptr);
	mgr->setMouseContextMenu(this);

	layout.setExcludeFromLayout(true);

	auto mousePos = ax_floor(mgr->mouseWorldPos());
	setWorldPos(mousePos);

	layout.setFitContentW().setFitContentH();
	_renderView.createRenderContext();
	setWindowActive(true);
}

void axUIContextMenu::onMouseEvent(axUIMouseEvent& ev) {
	using Type = axUIMouseEvent::Type;
	if (ev.type == Type::Up) {
		if (ev.pressedTime.toSeconds() > 1) {
			setNeedToDestroy();
		}
	}
}

void axUIContextMenu::addItem(axStrView text, axObject* obj, Func func) {
	auto* item = ax_new MenuItem(&_menu, this, obj, func);
	item->setText(text);
}

void axUIContextMenu::onSetNeedToRender() {
	_renderView.setNeedToRender();
}

void axUIContextMenu::onRenderChildren(axRenderRequest& req) {
	// block render children
	// render should be from RenderView::onRenderContextRender() rather than from parent axUIView
}

void axUIContextMenu::onWindowActive(bool b) {
	if (!b) {
		setNeedToDestroy();
	}
}

void axUIContextMenu::showOnScreen() {
	if (auto* p = parent()) {
		moveWithinWorldRect(p->uiScreen().worldRect());
	}
	setWindowActive(true);
}

void axUIContextMenu::RenderView::onRenderContextRender(axRenderRequest& req) {
	Base::onRenderContextRender(req);
}

axUIContextMenu::MenuItem::MenuItem(axUIView* parent, axUIContextMenu* owner, axObject* obj, Func func)
	: Base(parent)
	, _owner(owner)
{
	_delegate.bind(obj, func);
	setPadding(2);
	layout.setExpandW().setFitContentH();
}

void axUIContextMenu::MenuItem::setText(axStrViewU text) {
	_textMesh.setText(text);
}

void axUIContextMenu::MenuItem::onMeasureLayout(axUIMeasureRequest& req) {
	_textMesh.onMeasureLayout(req);
	req.result.min = ax_ceil(req.result.min) + _padding.total();
}

void axUIContextMenu::MenuItem::onMouseEvent(axUIMouseEvent& ev) {
	using Type = axUIMouseEvent::Type;
	using Button = axUIMouseEvent::Button;
	
	if (_owner) {
		 _owner->setWindowActive(true);
	}

	switch (ev.type) {
		case Type::Down: {
			if (ev.button == Button::Left ) {
				_isDown = true;
			}
		}break;

		case Type::Up: {
			_delegate.emit();
			_owner->setNeedToDestroy();
		}break;
	}
}

void axUIContextMenu::MenuItem::onRender(axRenderRequest& req) {
	req.ui.drawRect(AX_LOC, localRect(), isMouseHover() ? axColor(.5f, .5f, .9f) : axColor(.5f, .5f, .5f));
	_textMesh.renderInRect(req, contentRect());
}

axUIContextMenu::Menu::Menu(axUIView* parent)
	: Base(parent)
{
}
