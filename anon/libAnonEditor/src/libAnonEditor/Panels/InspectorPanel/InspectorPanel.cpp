#include <libanonEditor/App/EditorApp.h>
#include "InspectorPanel.h"

namespace Anon{ namespace Editor {

InspectorPanel::InspectorPanel(axUIDockZone* zone) 
	: Base(zone)
	, _objectGroup(this)
{
	setTitle("Inspector");
	setPadding(8);

	_objectGroup.reloadContent();
	setNeedToRender();
}

void InspectorPanel::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.type == axUIMouseEvent::Type::Down) {
//		axUIApp::s_instance()->rootView()->dump();
//		axUIFontManager::s_instance()->glypchCache()->_packer.debugStep++;
		setNeedToRender();
	}
}

void InspectorPanel::onEditorSelectionChanged() {
	_objectGroup.reloadContent();
}

void InspectorPanel::onAnonContextChanged(AnonContext::ChangedFlags flags) {
	using F = AnonContext::ChangedFlags;
	if (ax_enum(flags).hasFlags(F::NewObject | F::DeleteObject)) {
		_objectGroup.reloadContent();
	}
}

#if 0
#pragma mark ------ ObjectGroup ---------
#endif

InspectorPanel::ObjectGroup::ObjectGroup(InspectorPanel* parent) 
	: Base(parent)
{
	_inspectorPanel = parent;
	boxLayout.setSpacing(4);
}

void InspectorPanel::ObjectGroup::addObject(AnonObject* object) {
	if (!object) {
		AX_ASSERT(false);
		return;
	}

	if (_objects.contains(object)) return;
	_objects.emplaceBack(object);

	auto* type = ax_typeof(object);
	auto* view = findOrNewObjectInspector(type);
	if (!view) return;

	view->addObject(object);
}

void InspectorPanel::ObjectGroup::onReloadContent() {
	_objects.clear();
	clearChildren();

	auto* ed = EditorApp::s_instance();
	auto selView = ed->selection.current();

	for (auto& sel : selView) {
		if (auto* obj = sel.getObject()) {
			addObject(obj);
		}
	}

	for (auto& c : children()) {
		auto* v =  ax_type_cast<ObjectInspector>(&c);
		if (!v) continue;
		v->reloadContent();
	}

	setNeedToRender();
}

InspectorPanel::ObjectInspector* InspectorPanel::ObjectGroup::newObjectInspector(const axType* objectType) {
	auto* app = EditorApp::s_instance();

	ObjectInspector::CreateDesc desc;
	desc.parent = this;
	desc.objectType = objectType;

	auto* view = app->modules.editorUI.createObjectInspector(AX_LOC, ax_scope_allocator(), desc);
	view->evRelatedObject.bind(this, &This::onEvRelatedObject);
	view->setDebugName(objectType->name());
	return view;
}

InspectorPanel::ObjectInspector* InspectorPanel::ObjectGroup::findObjectInspector(const axType* objectType) {
	for (auto& c : children()) {
		auto* view =  ax_type_cast<ObjectInspector>(&c);
		if (!view || view->objectType() != objectType) continue;
		return view;
	}
	return nullptr;
}

 InspectorPanel::ObjectInspector* InspectorPanel::ObjectGroup::findOrNewObjectInspector(const axType* objectType) {
	 auto* view = findObjectInspector(objectType);
	 if (!view) {
		view = newObjectInspector(objectType);
	 }
	return view;
}

}} //namespace
