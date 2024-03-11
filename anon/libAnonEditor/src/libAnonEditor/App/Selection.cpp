#include "Selection.h"
#include "EditorApp.h"

namespace Anon { namespace Editor {

void SelectionItem::onFormat(axFormat& f) const {
	f << objectId;
	if (auto* obj = getObject()) {
		f << obj->name();
	}	
}

void Selection::select(AnonObject* obj) {
	AnonObject* list[] = {obj};
	select(axSpan_make(list));
}

void Selection::select(axSpan<AnonObject*> entities) {
	axArray<Item, 512> items;
	items.reserve(entities.size());
	for (auto& p : entities) {
		if (!p) continue;
		auto& dst = items.emplaceBack();
		dst.objectId = p->objectId();
	}
	select(items);
}

void Selection::_select(axSpan<Item> items) {
	auto cmd = axUPtr_new(SelectCommand(_current));
	cmd->after = items;
	auto* mgr = AppCommandManager::s_instance();
	mgr->addCommand(cmd.detach());
}

bool Selection::has(AnonObject* obj) {
	for (auto& e : _current) {
		if (obj == e.getObject())
			return true;
	}
	return false;
}

void Selection::SelectCommand::onUndo() {
	auto& sel = EditorApp::s_instance()->selection;
	sel._current = before;

//	AX_DUMP_VAR(sel._current);

	sel.evChanged.emit();
	AnonContext::s_instance()->setComputeNeeded();
}

void Selection::SelectCommand::onRedo() {
	auto& sel = EditorApp::s_instance()->selection;
	sel._current = after;

//	AX_DUMP_VAR(sel._current);

	sel.evChanged.emit();
	AnonContext::s_instance()->setComputeNeeded();
}

void Selection::SelectCommand::onFormat(axFormat& f) const {
	f << "Selection " << after;
}

}} // namespace
