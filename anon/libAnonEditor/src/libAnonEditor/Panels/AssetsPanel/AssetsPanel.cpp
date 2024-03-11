#include "AssetsPanel.h"
#include <libanonEditor/App/EditorApp.h>

namespace Anon{ namespace Editor {

AssetsPanel::AssetsPanel(axUIDockZone* zone) 
	: Base(zone)
	, _treeView(this)
{
	setTitle("Assets");
}

void AssetsPanel::reload() {
	_treeView.clearItems();

	auto* app = EditorApp::s_instance();
	auto* db = app->assetDatabase();

	_treeView.clearItems();	
	auto* rootItem = _addItem(db->root(), nullptr);
	rootItem->setExpand(true);
}

void AssetsPanel::onRenderChildren(axRenderRequest& req) {
	Base::onRenderChildren(req);
	//req.ui.drawRect(AX_LOC, localRect(), axColor(0,0,0, 0.5f));
	//req.drawStatistics({0,0});
}

AssetsPanel::Item* AssetsPanel::_addItem(AssetDatabaseEntry* entry, Item* parent) {
	if (!entry) return nullptr;

	auto* item = ax_new Item(parent);
	item->setLabel(entry->name());
//	item->setExpand(false);

	for (auto& c : entry->children()) {
		_addItem(&c, item);
	}
	return item;
}

void AssetsPanel::onUpdateLayout() {
	auto rc = contentRect();
	_treeView.setLayoutResultRect(rc);
}

void AssetsPanel::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.type == axUIMouseEvent::Type::Down) {
		setNeedToRender();
	}
}

AssetsPanel::TreeView::TreeView(AssetsPanel* panel)
	: Base(panel)
	, _panel(panel)
{
}

}} //namespace
