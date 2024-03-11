#pragma once

#include "../EditorPanel.h"

namespace Anon{ namespace Editor {

class SceneOutlinerPanel : public EditorPanel {
	AX_RTTI_CLASS(SceneOutlinerPanel, EditorPanel)
public:
	SceneOutlinerPanel(axUIDockZone* zone);

	virtual void onUpdateLayout() override;
	virtual void onEditorSelectionChanged() override;
	virtual void onAnonContextChanged(AnonContext::ChangedFlags flags) override;

	void reload();

	class Item : public axUITreeViewItem {
		AX_RTTI_CLASS(Item, axUITreeViewItem)
	public:
		Item(axUITreeViewItem* parent) : Base(parent) { object.setOwner(this); }

		virtual void onMouseEvent(axUIMouseEvent& ev) override;
		axLink<SCN_Object> object;
	};

	class TreeView : public axUITreeView {
		AX_RTTI_CLASS(TreeView, axUITreeView)
	public:
		using Item = SceneOutlinerPanel::Item;

		TreeView(SceneOutlinerPanel* outliner);

		SceneOutlinerPanel* _outliner = nullptr;
	};

	void _addObject(SCN_Object* object, Item* parent, bool isRoot);

private:
	Item* _getTreeViewItem(AnonObject* obj);

	TreeView	_treeView;
};

}} //namespace
