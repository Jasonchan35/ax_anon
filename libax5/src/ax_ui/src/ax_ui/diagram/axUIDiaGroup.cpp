#include "axUIDiaGroup.h"

axUIDiaGroup::axUIDiaGroup(axUIDiaGroup* parent)
	: axUIDiaGroup(static_cast<axUIView*>(parent))
{
}

axUIDiaGroup::axUIDiaGroup(axUIView* parent)
	: Base(parent)
	, _titleBar(this)
	, _childNodeView(this)
{
	setLayoutHandler(&_vboxLayoutHandler);

	_titleBar.setText("Group Title");
	_titleBar.layout.setExpandW();
	_titleBar.setBgColor(.3f, 0.3f, 0.3f, 1);

	setWantMouseDragEvent(false);
	setBgColor(.4f, .4f, .4f, .5f);
	setBorderColor(0,0,0,1);

	layout.setFitContentW();
	layout.setFitContentH();
}

void axUIDiaGroup::clearChildNodes() {
	_childNodeView.clearChildren();
}

void axUIDiaGroup::setTitle(axStrView title) { _titleBar.setText(title); }

void axUIDiaGroup::onRender(axRenderRequest& req) {
	req.ui.drawRect(AX_LOC, contentRect(), isSelected() ? axColor(.5f, .5f, .2f) : _bgColor);
	req.ui.drawRectBorder(AX_LOC, localRect(), axMargin2f(1), axColor(1,1,1,1));
}

void axUIDiaGroup::onUpdateLayout() {
	auto oldRect = _childNodeView.rect();
	updateChildrenLayout();
	auto newRect = _childNodeView.rect();

	auto deltaPos  = newRect.pos  - oldRect.pos;

	_childNodeView.offsetPos(-deltaPos);
	offsetPos(deltaPos);

	auto& m = layout.measured();
	setSize(m.min);

	// vbox
	Base::onUpdateLayout();
}

void axUIDiaGroup::onUpdateConnectionPosition() {
	Base::onUpdateConnectionPosition();

	for (auto& c : _childNodeView.children()) {
		if (auto* node = ax_type_cast<axUIDiaNodeBase>(&c)) {
			node->updateConnectionPosition();
		}
	}
}

axUIDiaGroup::ChildNodeView::ChildNodeView(axUIDiaGroup* parent) 
	: Base(parent)
	, _owner(parent)
{
	setLayoutHandler(nullptr);
	setWantMouseDragEvent(false);
	setPadding(20);

	layout.setFitContentW();
	layout.setFitContentH();
}

void axUIDiaGroup::ChildNodeView::onMeasureLayout(axUIMeasureRequest& req) {
	auto rc = getChildrenBounds();
	req.result.min = rc.size;
}

void axUIDiaGroup::ChildNodeView::onUpdateLayout() {
	updateChildrenLayout();

	auto rc = getChildrenBounds() + _padding;
	offsetPos(rc.pos);
	setSize(rc.size);

	for (auto& c : children()) {
		c.offsetPos(-rc.pos);
	}

	Base::onUpdateLayout();
}