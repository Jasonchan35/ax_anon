#pragma once

#include "../EditorPanel.h"

namespace Anon{ namespace Editor {

class ConsolePanel : public EditorPanel {
	AX_RTTI_CLASS(ConsolePanel, EditorPanel)
public:
	ConsolePanel(axUIDockZone* zone);

	virtual void onMouseEvent(axUIMouseEvent& ev) override;
};

}} //namespace
