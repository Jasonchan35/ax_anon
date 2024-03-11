#include "EditorPanel.h"
#include "../App/AppCommand.h"

namespace Anon{ namespace Editor {

EditorPanel::EditorPanel(axUIDockZone* zone)
	: Base(zone) 
{
	setBgColor(0.15f, 0.15f, 0.15f);

	auto* app = EditorApp::s_instance();
	app->selection.evChanged.bind(this,
		[this]() 
	{
		onEditorSelectionChanged();
	});

	auto* anon = AnonContext::s_instance();
	anon->bindChangedFlagsEvent(this, 
		[this](auto& flags) 
	{
		onAnonContextChanged(flags);
	});
}

}} //namespace
