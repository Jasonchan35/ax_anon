#pragma once

#include "Selection.h"
#include "../Asset/AssetDatabase.h"
#include "../Scene/SCN_EditorModule.h"
#include "../Geometry/GEO_EditorModule.h"
#include "../EditorUI/EdUIModule.h"
#include "../Panels/SceneOutlinerPanel/SceneOutlinerPanel.h"

namespace Anon{ namespace Editor {

class EditorMainWindow;
class EditorApp : public axUIDockApp {
	AX_RTTI_CLASS(EditorApp, axUIDockApp)
public:
	ax_DOWNCAST_GET_INSTANCE()

	EditorApp();
	~EditorApp();

	virtual void onCreate() override;

	EditorMainWindow*	mainWindow() { return _mainWin.ptr(); }

	virtual void onUIKeyEvent(UIKeyEvent& ev);

	Selection	selection;
	axStrView	assetsDir() { return _assetsDir; }
	AssetDatabase* assetDatabase() { return _assetDatabase.ptr(); }

	struct Modules {
		EdUIModule		editorUI;
		SCN_EditorModule	scene;
		GEO_EditorModule	geo;
		void create();
	};

	Modules		modules;

	void loadScene(axStrView filename);
	void openDiagramPanel(Diagram* dia);

	axEvent<void (Diagram*)>	evOpenDiagramPanel;

	void addShortcutKey(axUIShortcutKey key, axObject* obj, axFunc<void ()> func);

protected:
	virtual void onHandleCustomAppEvent() override;
	virtual bool onKeyboardShortcut(UIKeyEvent& ev) override;

private:
	struct ShortcutTable {
		struct Entry {
			axEvent<void()>	ev;
		};
		axDict<axUIShortcutKey, Entry>	_dict;
	};

	ShortcutTable				_shortcutTable;

	AnonContext					_anonContext;
	axString					_assetsDir;
	axUPtr<AssetDatabase>		_assetDatabase;

	axLink<EditorMainWindow>	_mainWin;
	AppCommandManager			_undoManager;
};

}} //namespace
