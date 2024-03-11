#pragma once

#include "../EditorPanel.h"

namespace Anon{ namespace Editor {

class AssetDatabaseEntry;

class AssetsPanel : public EditorPanel {
	AX_RTTI_CLASS(AssetsPanel, EditorPanel)
public:
	AssetsPanel(axUIDockZone* zone);

	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onUpdateLayout() override;

	void reload();

	class TreeView : public axUITreeView {
		AX_RTTI_CLASS(TreeView, axUITreeView)
	public:
		TreeView(AssetsPanel* panel);
		AssetsPanel* _panel = nullptr;
	};

	using Item = TreeView::Item;

	virtual void onRenderChildren(axRenderRequest& req) override;

private:
	Item* _addItem(AssetDatabaseEntry* entry, Item* parent);
	TreeView	_treeView;
};

}} //namespace
