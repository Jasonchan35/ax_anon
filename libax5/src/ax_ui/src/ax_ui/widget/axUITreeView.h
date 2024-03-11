#pragma once

#include "axUILabel.h"
#include "axUIScrollView.h"
#include "axUIButton.h"
#include "../base/axUISelectionList.h"

class axUITreeView;

class axUITreeViewColumn : public axUIView {
	AX_RTTI_CLASS(axUITreeViewColumn, axUIView);
public:
	axUITreeViewColumn(axUIView* parent);

friend class axUITreeView;
protected:
	axString _name;
};

class axUITreeViewItem : public axUIVBoxView, public axUISelectionListNode<axUITreeViewItem> {
	AX_RTTI_CLASS(axUITreeViewItem, axUIVBoxView);
public:
	using Item = axUITreeViewItem;
	using TreeView = axUITreeView;
	using SelectionNode = axDListNode<Item>;
	using Column = axUITreeViewColumn;

	class ItemRow;

	class ExpandIcon : public axUIToggleButton {
		AX_RTTI_CLASS(ExpandIcon, axUIToggleButton)
	public:
		ExpandIcon(ItemRow* item);
		virtual void onRender(axRenderRequest& req) override;
		virtual void onMouseEvent(axUIMouseEvent& ev) override;
	private:
		ItemRow* _row = nullptr;
	};

	class ItemRow : public axUIHBoxView {
		AX_RTTI_CLASS(ItemRow, axUIHBoxView)
	public:
		ItemRow(Item* item);
		virtual void onRender(axRenderRequest& req) override;
		virtual void onMouseEvent(axUIMouseEvent& ev) override;

		Item*	item() { return _item; }

	friend class axUITreeViewItem;
	protected:
		Item*		_item = nullptr;
		ExpandIcon	_expandIcon;
		axUILabel	_label;
	};

	class SubTree : public axUIVBoxView {
		AX_RTTI_CLASS(SubTree, axUIVBoxView)
	public:
		SubTree(Item* item);
		Item*	item() { return _item; }
	private:
		Item*	_item = nullptr;
	};

	axUITreeViewItem(Item* parent);
	
	void	setLabel(axStrView text);
	bool	expand() const;
	void	setExpand(bool b);

	axInt	subItemCount() const;
	void	clearSubItems();

	TreeView*	treeView() { return _treeView; }

	ItemRow&	itemRow() { return _itemRow; }

friend class axUITreeView;
protected:
	axUITreeViewItem(TreeView* parent);
private:
	axUITreeViewItem(axUIView* parent);

	TreeView*		_treeView = nullptr;
	ItemRow			_itemRow;
	SubTree			_subTree;
	axInt			_level = -1;
};


class axUITreeView : public axUIView {
	AX_RTTI_CLASS(axUITreeView, axUIView)
public:
	using TreeView = axUITreeView;
	using Item = axUITreeViewItem;
	using ExpandIcon = axUITreeViewItem::ExpandIcon;
	using Selection = axUISelectionList<Item>;
	using Column = axUITreeViewColumn;

	axUITreeView(axUIView* parent);
	~axUITreeView();

			void	clearItems();

			Item*	rootItem	();

	Selection::Enumerator	selection()		{ return _selection.each(); }

			bool	setSelection(Item* item, bool fireEvent)				{ return setSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	setSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	addSelection(Item* item, bool fireEvent)				{ return addSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	addSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	removeSelection(Item* item, bool fireEvent)				{ return removeSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	removeSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	clearSelection(bool fireEvent);

	axDelegate<void ()> evSelectionChanged;

			float	itemIndent() const		{ return _itemIndent; }
			void	setItemIndent(float v)	{ _itemIndent = v; }

			void	setRowSpacing(float v);
			float	rowSpacing() const		{ return _rowSpacing; }

friend class axUITreeViewItem::ExpandIcon;
protected:
	axUIScrollView		_scrollView;
	axRenderMesh		_expandIconMesh;

private:

	Item				_rootItem;
	Selection			_selection;
	axIndirectArray<Column>	_columns;
	float _rowSpacing = 1;
	float _itemIndent = 16;
};
