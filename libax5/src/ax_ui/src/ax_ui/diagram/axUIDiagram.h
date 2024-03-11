#pragma once

#include "axUIDiaGroup.h"

class axUIDiagram : public axUIView {
	AX_RTTI_CLASS(axUIDiagram, axUIView)
public:
	using Item = axUIDiaItem;
	using SelectionList = axUISelectionList<Item>;

	axUIDiagram(axUIView* parent);
	virtual ~axUIDiagram();

	class ScrollableContent : public axUIView {
		AX_RTTI_CLASS(ScrollableContent, axUIView);
	public:
		ScrollableContent(axUIView* parent);
	};

	class Root : public axUIDiaGroup {
		AX_RTTI_CLASS(Root, axUIDiaGroup)
	public:
		Root(axUIView* parent);
		virtual void onRender(axRenderRequest& req) override;
		virtual void onMouseEvent(axUIMouseEvent& ev) override { forwardEventToParent(ev); }
	};

	void clearChildNodes();

	Root* root() { return &_root; }

	virtual bool onWillAcceptConnection(axUIDiaProp* src, axUIDiaProp* dst) { return false; }
//	virtual void onConnectProp(axUIDiaProp* src, axUIDiaProp* dst) {}
	virtual bool onAcceptConnection	(axUIDiaConnection* conn) { return true; }

	SelectionList::Enumerator	selection()						{ return _selection.each(); }

			bool	setSelection(Item* item, bool fireEvent)					{ return setSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	setSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	addSelection(Item* item, bool fireEvent)					{ return addSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	addSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	removeSelection(Item* item, bool fireEvent)					{ return removeSelection(axMutSpan<Item*>(&item, 1), fireEvent); }
			bool	removeSelection(axMutSpan<Item*> items, bool fireEvent);

			bool	clearSelection(bool fireEvent);

	axDelegate<void ()> evSelectionChanged;

			virtual axUIDiaConnection* onNewConnectionView(axUIDiaSocket* parent, axUIDiaSocket* target);

protected:
	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onUpdateLayout() override;

private:

	ScrollableContent	_scrollableContent;

	Root			_root;
	axRenderMesh	_gridMesh;

	float	_gridCellSize = 10;
	axInt	_gridCellCount = 0;
	Color	_gridColor;
	Color	_gridColor2;
	axInt	_gridColor2_Interval;

	SelectionList _selection;
};
