#include "EditorMainWindow.h"
#include "EditorApp.h"

namespace Anon{ namespace Editor {

EditorMainWindow::CreateDesc& EditorMainWindow::_createDesc() {
	static EditorMainWindow::CreateDesc desc;
	desc.isMainWindow = true;
	desc.visible = false;

	// desc.resizable = false;
	// desc.minButton = false;
	// desc.maxButton = false;
	// desc.closeButton = false;
	return desc;
}

EditorMainWindow::EditorMainWindow() 
	: Base(EditorApp::s_instance()->rootView(), _createDesc())
	, _toolbar(headerArea())
{
	axTempString title("Test");

	auto* app = EditorApp::s_instance();
	auto* renderer = app->renderer();
	title.appendFormat(" - {?}, MT: {?}, VSync: {?}", renderer->api(), renderer->multithread(), renderer->vsync());
	if (renderer->rayTracing()) {
		title.appendFormat(", RayTracing");
	}
	setWindowTitle(title);
	setWindowActive(true);
}

EditorMainWindow::ToolBar::ToolBar(axUIView* parent)
	: Base(parent)
	, _saveSceneButton(this, "Save Scene")
	, _revertSceneButton(this, "Revert Scene")
	, _undoButton(this, "Undo")
	, _redoButton(this, "Redo")
	, _dumpUndoListButton(this, "Dump Undo List")
	, _clearUndoListButton(this, "Clear Undo List")
{
	_revertSceneButton.evClick.bind(this, [](auto& ev){ SCN_System::s_instance()->revertCurrentSceneFromFile(); });
	_saveSceneButton.evClick.bind(this, [](auto& ev){ SCN_System::s_instance()->saveCurrentSceneFile(); });

	_undoButton.evClick.bind(this, [](auto& ev) { AppCommandManager::s_instance()->undo(); });
	_redoButton.evClick.bind(this, [](auto& ev) { AppCommandManager::s_instance()->redo(); });

	_dumpUndoListButton.evClick.bind(this, [](auto& ev) {
		AppCommandManager::s_instance()->dumpUndoList();
	});

	_clearUndoListButton.evClick.bind(this, [](auto& ev) {
		AppCommandManager::s_instance()->clearUndoList();
	});
}

}} //namespace
