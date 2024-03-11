#include "axUITreeView.h"

#if 0
#pragma mark ------ column ---------
#endif

axUITreeViewColumn::axUITreeViewColumn(axUIView* parent) 
	: Base(parent) 
{
}

#if 0
#pragma mark ------ SubTree ---------
#endif

axUITreeViewItem::SubTree::SubTree(Item* item)
	: Base(item)
	, _item(item)
{
}

#if 0
#pragma mark ------ Item ---------
#endif

axUITreeViewItem::axUITreeViewItem(axUIView* parent)
	: Base(parent)
	, _itemRow(this)
	, _subTree(this)
{
	layout.setFitContentH();
}

axUITreeViewItem::axUITreeViewItem(TreeView* parent) 
	: Item(static_cast<axUIView*>(parent))
{
	// this is Root item
	_itemRow.setVisible(false);
	_treeView = parent;
}

axUITreeViewItem::axUITreeViewItem(Item* parent)
	: Item(&parent->_subTree)
{
	_level = parent->_level + 1;
	_treeView = parent->treeView();
	_itemRow.setPadding(Margin2(0, 0, 0, _level * _treeView->itemIndent()));
	setExpand(true);
}

void axUITreeViewItem::setExpand(bool b) {
	_itemRow._expandIcon.setIsDown(b);
	_subTree.setVisible(b);
}

axInt axUITreeViewItem::subItemCount() const {
	return _subTree.childCount();
}

void axUITreeViewItem::clearSubItems() {
	_subTree.clearChildren();
}

void axUITreeViewItem::setLabel(axStrView text) {
	_itemRow._label.setText(text);
}

#if 0
#pragma mark ------ ItemRow ---------
#endif

axUITreeViewItem::ItemRow::ItemRow(Item* item)
	: Base(item)
	, _item(item)
	, _expandIcon(this)
	, _label(this)
{
	setAcceptFocus(true);
	layout.setFitContentH();

	_label.layout.setExpandW();

	_expandIcon.evClick.bind(this, [this](auto& ev) {
		_item->setExpand(_expandIcon.isDown());
	});
}

void axUITreeViewItem::ItemRow::onRender(axRenderRequest& req) {
	Color color(0,0,0,0);

	if (isMouseHover() || _label.isMouseHover() || _expandIcon.isMouseHover()) {
		color.set(0.8f, 0.4f, 0.5f, 0.5f);
	}else if (_item->isSelected()) {
		color.set(0.2f, 0.4f, 0.5f);
	}

	if (color.a > 0) {
		req.ui.drawRect(AX_LOC, localRect(), color);
	}
}

void axUITreeViewItem::ItemRow::onMouseEvent(axUIMouseEvent& ev) {
	using Type = axUIMouseEvent::Type;
	using Modifier = axUIMouseEvent::Modifier;

	if (ev.type == Type::Down) {
		setFocus(true);
		auto* tv = _item->treeView();

		if (ax_enum(ev.modifier).hasFlags(Modifier::Ctrl)) {
			if (_item->isSelected()) {
				tv->removeSelection(_item, true);
			}else{
				tv->addSelection(_item, true);
			}
		} else {
			tv->setSelection(_item, true);
		}
	}

	Base::onMouseEvent(ev);
}

#if 0
#pragma mark ------ axUITreeView ---------
#endif

axUITreeView::axUITreeView(axUIView* parent) 
	: Base(parent)
	, _scrollView(this)
	, _rootItem(this)
{
	_scrollView.setScrollableContent(&_rootItem);

	{
		const float d = 5;
		using Vertex = axRenderVertex_PosColor;
		using Index  = axUInt16;
		_expandIconMesh.clear();

		auto&& added = _expandIconMesh.addVertices<Vertex, Index>(axRenderPrimitiveType::Triangles, 3);
		auto& v = added.vertices;

		float y = ax_floor(d *  0.8f);
		float x = ax_floor(d * -0.5f);

		auto color = axColorRGBAh_make(1,1,1,1);

		v[0].pos.set(d,  0, 0);	v[0].color[0] = color;
		v[1].pos.set(x,  y, 0);	v[1].color[0] = color;
		v[2].pos.set(x, -y, 0);	v[2].color[0] = color;
	}
}

axUITreeView::~axUITreeView() {
	clearSelection(false);
}

void axUITreeView::clearItems() {
	_rootItem.clearSubItems();
}

axUITreeView::Item* axUITreeView::rootItem() {
	return &_rootItem;
}

void axUITreeView::setRowSpacing(float v) {
	_rowSpacing = v;
	setNeedToLayout();
}

bool axUITreeView::clearSelection(bool fireEvent) {
	if (!_selection.clear())
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUITreeView::setSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.set(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUITreeView::addSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.add(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

bool axUITreeView::removeSelection(axMutSpan<Item*> items, bool fireEvent) {
	if (!_selection.remove(items))
		return false;
	if (fireEvent) evSelectionChanged.emit();
	setNeedToRender();
	return true;
}

#if 0
#pragma mark ------ ExpandIcon ---------
#endif

axUITreeViewItem::ExpandIcon::ExpandIcon(ItemRow* row)
	: Base(row, "")
	, _row(row)
{
	setBorderWidth(0);
	layout.setMin(20, 20);
}

void axUITreeViewItem::ExpandIcon::onRender(axRenderRequest& req) {
	if (!_row->item()->subItemCount()) {
		return;
	}

	auto center = localRect().center();

	auto mat = Mat4::s_translate(center)
			 * Mat4::s_rotateDegZ(isDown() ? 90.0f : 0.0f);

	req.ui.drawMesh(AX_LOC, _row->item()->treeView()->_expandIconMesh, mat);
}

void axUITreeViewItem::ExpandIcon::onMouseEvent(axUIMouseEvent& ev) {
	if (!_row->item()->subItemCount()) {
		forwardEventToParent(ev);
		return;
	}	
	Base::onMouseEvent(ev);
}

