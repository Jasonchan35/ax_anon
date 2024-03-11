#pragma once

#include "../EditorUI/EdUICommon.h"

namespace Anon{ namespace Editor {

class EditorApp;
class EditorMainWindow : public axUIDockWindow {
	AX_RTTI_CLASS(EditorMainWindow, axUIDockWindow)
public:
	EditorMainWindow();

	class ToolBar : public HBoxView {
		AX_RTTI_CLASS(ToolBar, HBoxView);
	public:
		ToolBar(axUIView* parent);
		EdUIButton	_saveSceneButton;
		EdUIButton	_revertSceneButton;
		EdUIButton	_undoButton;
		EdUIButton	_redoButton;
		EdUIButton	_dumpUndoListButton;
		EdUIButton	_clearUndoListButton;
	};

private:
	static CreateDesc& _createDesc();
	ToolBar		_toolbar;
};

}} //namespace
