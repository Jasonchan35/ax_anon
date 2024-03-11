#include "SceneOutlinerPanel.h"
#include "../../App/EditorApp.h"

namespace Anon{ namespace Editor {

SceneOutlinerPanel::SceneOutlinerPanel(axUIDockZone* zone) 
	: Base(zone)
	, _treeView(this)
{
	setTitle("Scene Outliner");
}

void SceneOutlinerPanel::onUpdateLayout() {
	auto rc = contentRect();
	_treeView.setLayoutResultRect(rc);
}

SceneOutlinerPanel::Item* SceneOutlinerPanel::_getTreeViewItem(AnonObject* obj) {
	if (!obj) return nullptr;
	for (auto& link : obj->Linkable_each()) {
		if (auto* item = link.owner<Item>()) {
			if (item->treeView() == &_treeView)
				return item;
		}
	}
	return nullptr;
}

void SceneOutlinerPanel::onEditorSelectionChanged() {
	_treeView.clearSelection(false);

	auto* app = EditorApp::s_instance();
	for (auto& s : app->selection.current()) {
		if (auto* item = _getTreeViewItem(s.getObject())) {
			_treeView.addSelection(item, false);
		}
	}
}

void SceneOutlinerPanel::onAnonContextChanged(AnonContext::ChangedFlags flags) {
	using F = AnonContext::ChangedFlags;
	if (ax_enum(flags).hasAnyFlag(F::NewObject | F::DeleteObject | F::RenameObject)) {
		reload();
	}
}

void SceneOutlinerPanel::reload() {
	_treeView.clearItems();
	auto* root = SCN_System::s_instance()->rootObject();
	_addObject(root, nullptr, true);
}

void SceneOutlinerPanel::_addObject(SCN_Object* object, Item* parent, bool isRoot) {
	if (!object) return;

	Item* item = nullptr;

	if (!isRoot) {
		item = ax_new Item(parent ? parent : _treeView.rootItem());
		item->object = object;
		item->setLabel(object->name().toString());

		auto* app = EditorApp::s_instance();
		if (app->selection.has(object)) {
			_treeView.addSelection(item, false);
		}
	}

	for (auto& c : object->children()) {
		_addObject(&c, item, false);
	}
}

SceneOutlinerPanel::TreeView::TreeView(SceneOutlinerPanel* outliner) 
	: Base(outliner)
{
	_outliner = outliner;

	evSelectionChanged.bind(this, [this]() {
		axArray<AnonObject*, 64> list;
		for (auto& t : selection()) {
			if (auto* item = ax_type_cast<Item>(&t)) {
				list.emplaceBack(item->object);
			}
		}
		EditorApp::s_instance()->selection.select(list);
	});

}

void SceneOutlinerPanel::Item::onMouseEvent(axUIMouseEvent& ev) {
	Base::onMouseEvent(ev);

	if (ev.isRightClick()) {
		auto menu = ax_new axUIContextMenu(this);
		auto objId = object->objectId();
		menu->addItem("Create Object", this, [objId](){
			auto* parent = SCN_Object::s_find(objId);
			auto* obj = SCN_System::s_instance()->newObject(parent);
			obj->setName("New Object");
		});
		menu->setWorldPos(ev.worldPos);
		menu->showOnScreen();
	}
}

}} //namespace
