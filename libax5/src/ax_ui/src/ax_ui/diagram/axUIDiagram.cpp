#include "axUIDiagram.h"
#include "../base/axUIApp.h"
#include "../menu/axUIContextMenu.h"

axUIDiagram::ScrollableContent::ScrollableContent(axUIView* parent) 
	: Base(parent) 
{
	setLayoutHandler(nullptr);
	setAcceptFocus(true);
}

axUIDiagram::axUIDiagram(axUIView* parent)
	: Base(parent)
	, _scrollableContent(this)
	, _root(&_scrollableContent)
	, _gridCellSize(20)
	, _gridCellCount(0)
	, _gridColor(.22f, .22f, .22f)
	, _gridColor2(.25f, .25f, .25f)
	, _gridColor2_Interval(5)
{
	setLayoutHandler(nullptr);
	setWantMouseWheelEvent(true);
}

axUIDiagram::~axUIDiagram() {
	clearChildNodes();
}

void axUIDiagram::clearChildNodes() {
	clearSelection(false);
	_root.clearChildNodes();
}

bool axUIDiagram::clearSelection(bool fireEvent) {
	if (!_selection.clear())
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUIDiagram::setSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.set(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUIDiagram::addSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.add(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUIDiagram::removeSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.remove(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

axUIDiaConnection* axUIDiagram::onNewConnectionView(axUIDiaSocket* parent, axUIDiaSocket* target) {
	return ax_new axUIDiaConnection(parent, target);
}

void axUIDiagram::onRender(axRenderRequest& req) {
	//axTempString str;
	//_scrollableContent.onGetDebugInfo(str);
	//req.ui.drawText(AX_LOC, str, axVec2f(10, 10));

	{ // grid
		auto pos = Vec2((_gridCellCount / 2) * _gridCellSize);
		auto s = _gridCellSize * _gridColor2_Interval;
		pos += ax_fmod(_scrollableContent.pos(), Vec2(s)) - s;
		auto mat = Mat4::s_translate(pos);

		req.ui.drawMesh(AX_LOC, _gridMesh, mat);
	}

	if (hasFocus()) {
		req.ui.drawRectBorder(AX_LOC, localRect(), {2}, axColor::kYellow());
	}
}

void axUIDiagram::onMouseEvent(axUIMouseEvent& ev) {
	using Button = axUIMouseEvent::Button;
	using Type   = axUIMouseEvent::Type;

	switch (ev.type) {
		case Type::DragBegin: {
			if(ev.pressedButtons == Button::Middle || ev.pressedButtons == Button::Right) {
				axUIApp::s_instance()->setCursor(axUICursorType::OpenHand);
			}
		}break;
		case Type::Dragging: {
			if(ev.pressedButtons == Button::Middle || ev.pressedButtons == Button::Right) {
				_scrollableContent.offsetPos(ev.deltaPos);
				setNeedToLayout();
			}
		}break;
		case Type::DragEnd: {
				axUIApp::s_instance()->setCursor(axUICursorType::Arrow);
		}break;
		
		case Type::Wheel: {
			_scrollableContent.offsetPos(ax_ceil(ev.wheelDelta * 0.05f));
		}break;
	}
}

void axUIDiagram::onUpdateLayout() {
	{	// updateGrid
		auto v = this->size() / _gridCellSize + (float)_gridColor2_Interval;
		auto c = ax_max((axInt)v.x, (axInt)v.y, _gridCellCount) + _gridColor2_Interval;

		if (c > _gridCellCount) {
			_gridCellCount = c;
			_gridMesh.edit().createGrid(axRenderVertex_PosColor::s_desc(),
										_gridCellSize, _gridCellCount,
										_gridColor2, _gridColor, _gridColor2,
										_gridColor2_Interval);
		}
	}
	Base::onUpdateLayout();
}

axUIDiagram::Root::Root(axUIView* parent) 
	: Base(parent)
{
	_titleBar.setText("axUIDiagram::Root");
	_titleBar.setVisible(false);
	setBgColor(0,0,0,0);
	setBorderColor(0,0,0,0);
	layout.setFitContentW();
	layout.setFitContentH();
}

void axUIDiagram::Root::onRender(axRenderRequest& req) {
	_childNodeView.render(req);
}
