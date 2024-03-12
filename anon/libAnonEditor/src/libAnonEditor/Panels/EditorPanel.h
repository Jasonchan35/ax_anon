#pragma once

#include <ax_ui/dock/axUIDockPanel.h>
#include <libAnonEditor/App/AppCommand.h>

namespace Anon{ namespace Editor {

class Selection;

class EditorPanel : public axUIDockPanel {
	AX_RTTI_CLASS(EditorPanel, axUIDockPanel)
public:
	EditorPanel(axUIDockZone* zone);

	virtual void onEditorSelectionChanged() {};
	virtual void onAnonContextChanged(AnonContext::ChangedFlags flags) {};
};

}} //namespace
