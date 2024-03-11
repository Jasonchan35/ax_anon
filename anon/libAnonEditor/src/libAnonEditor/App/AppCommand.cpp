#include "AppCommand.h"
#include "EditorApp.h"

namespace Anon { namespace Editor {

void AppCommandGroup::undo() {
	auto* mgr = AppCommandManager::s_instance();
	for (auto& cmd : _cmdList.revEach()) {
		cmd.undo();
		mgr->evCommand.emit(cmd, true);
	}
}

void AppCommandGroup::redo() {
	auto* mgr = AppCommandManager::s_instance();
	for (auto& cmd : _cmdList) {
		cmd.redo();
		mgr->evCommand.emit(cmd, false);
	}
}

void AppCommandGroup::onFormat(axFormat& f) const {
	f << "\nAppCommandGroup [" << _name << "] isPreview=" << isPreview << "\n";
	f << _cmdList;

}

AppCommandManager* AppCommandManager_instance;

AppCommandManager::AppCommandManager() {
	AX_ASSERT(AppCommandManager_instance == nullptr);
	AppCommandManager_instance = this;
	_debugDump = false;

	auto* app = EditorApp::s_instance();
	{
		using M = axUIEventModifier;
		using Key = axUIKeyCode;
		app->addShortcutKey({M::Ctrl,            Key::Z}, this, [this](){ undo(); });
		app->addShortcutKey({M::Ctrl | M::Shift, Key::Z}, this, [this](){ redo(); });
	}
}

AppCommandManager* AppCommandManager::s_instance() {
	return AppCommandManager_instance;
}

void AppCommandManager::newGroup(axStrView name) {
	_redoList.clear();
	auto* p = ax_new AppCommandGroup();
	_undoList.append(p);
	p->_name = name;
}

void AppCommandManager::addCommand(AppCommand* cmd, bool isPreview) {
	newGroup("");
	auto* t = _undoList.tail();
	t->_cmdList.append(cmd);
	t->isPreview = isPreview;
	cmd->isPreview = isPreview;
	cmd->redo();
	evCommand.emit(*cmd, false);

	if (_debugDump) {
		AX_LOG("---- addCommand ----");
		AX_DUMP_VAR(_undoList);
	}

	while (_undoList.size() > _maxUndoCount) {
		_undoList.popHead();
	}
}

bool AppCommandManager::undoPreview() {
	if (!_undoList) return false;
	auto* p = _undoList.tail();
	if (!p) return false;
	if (!p->isPreview) return false;

	p->undo();
	_redoList.insert( _undoList.popTail() );

	if (_debugDump) {
		AX_LOG("---- undo preview ----");
		AX_DUMP_VAR(_undoList);
	}

	return true;
}

bool AppCommandManager::undo() {
	auto* p = _undoList.tail();
	if (!p) return false;

	p->undo();
	_redoList.insert( _undoList.popTail() );

	if (_debugDump) {
		AX_LOG("---- undo ----");
		AX_DUMP_VAR(_undoList);
	}

	return true;
}

bool AppCommandManager::redo() {
	auto* p = _redoList.head();
	if (!p) return false;

	p->redo();
	_undoList.append( _redoList.popHead() );

	if (_debugDump) {
		AX_LOG("---- redo ----");
		AX_DUMP_VAR(_undoList);
	}

	return true;
}

void AppCommandManager::dumpUndoList() {
	AX_LOG("--- undo list ----\n{?}", _undoList);
}

void AppCommandManager::clearUndoList() {
	_undoList.clear();
}

void AppCommand::onFormat(axFormat& f) const {
	f << ax_typeof(this)->name();
}

}} //namespace
