#pragma once

#include <ax_ui/widget/axUILabel.h>

class axUIButtonLogic;

class axUIButtonEvent {
public:
	axUIButtonEvent(axUIButtonLogic* button_) : button(button_) {}
	axUIButtonLogic*	button = nullptr;
	axUIMouseEvent*		mouseEvent = nullptr;
	axUIKeyEvent*		keyEvent = nullptr;
};

// Logic without visual
class axUIButtonLogic : public axUIView {
	AX_RTTI_CLASS(axUIButtonLogic, axUIView)
public:

	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	axDelegate<void (axUIButtonEvent& ev)> evClick;

	void setIsToggleButton(bool b) { _isToggleButton = b; }
	bool isToggleButton() const { return _isToggleButton; }

	void setIsDown(bool b) { onSetIsDown(b); }
	bool isDown() const { return _isDown; }

	virtual void onClick(axUIButtonEvent& ev) { evClick.emit(ev); }

	bool isPressed() const { return _isPressed; }

protected:
	axUIButtonLogic(axUIView* parent);
	virtual void onSetIsDown(bool b);

	void _setIsPressed(bool b);

private:
	bool _isDown = false;
	bool _isToggleButton = false;
	bool _isPressed = false;
};

class axUIButton : public axUIButtonLogic {
	AX_RTTI_CLASS(axUIButton, axUIButtonLogic)
public:
	axUIButton(axUIView* parent, axStrView labelText = "");
	virtual void onRender(axRenderRequest& req) override;

	axUILabel	label;
};

class axUIToggleButton : public axUIButton {
	AX_RTTI_CLASS(axUIToggleButton, axUIButton)
public:
	axUIToggleButton(axUIView* parent, axStrView labelText = "") : Base(parent, labelText) { setIsToggleButton(true); }
};

