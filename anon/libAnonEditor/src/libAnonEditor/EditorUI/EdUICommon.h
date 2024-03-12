#pragma once
#include <libAnon/Core/AnonObject.h>

namespace Anon { namespace Editor {

class EdUILabel : public axUILabel {
	AX_RTTI_CLASS(EdUILabel, axUILabel)
public:
	EdUILabel(axUIView* parent, StrView label = "") : Base(parent, label) {}
};

class EdUITextBox : public axUITextBox {
	AX_RTTI_CLASS(EdUITextBox, axUITextBox)
public:
	EdUITextBox(axUIView* parent) : Base(parent) {}

};

class EdUIButton : public axUIButton {
	AX_RTTI_CLASS(EdUIButton, axUIButton)
public:
	EdUIButton(axUIView* parent, axStrView text) : Base(parent, text) {}
};

class EdUIToggleButton : public EdUIButton {
	AX_RTTI_CLASS(EdUIButton, EdUIButton)
public:
	EdUIToggleButton(axUIView* parent, axStrView text) : Base(parent, text) {
		setIsToggleButton(true);
	}
};


class EdUIContextMenu : public axUIContextMenu {
	AX_RTTI_CLASS(EdUIContextMenu, axUIContextMenu)
public:
	EdUIContextMenu(axUIView* parent) : Base(parent) {}

};

}} //namespace
