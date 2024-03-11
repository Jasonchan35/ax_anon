#include "axUIButton.h"

axUIButtonLogic::axUIButtonLogic(axUIView* parent) 
	: Base(parent)
{
}

void axUIButtonLogic::onSetIsDown(bool b) {
	if (_isDown == b) return;
	_isDown = b;
	setNeedToRender();
}

void axUIButtonLogic::_setIsPressed(bool b) {
	if (_isPressed == b) return;
	_isPressed = b;
	setNeedToRender();
}

void axUIButtonLogic::onMouseEvent(axUIMouseEvent& ev) {
	switch (ev.type) {
		case axUIMouseEvent::Type::Down: {
			if (ev.button == axUIMouseEvent::Button::Left ) {
				_setIsPressed(true);
			}
		}break;

		case axUIMouseEvent::Type::Up: {
			if (ev.button == axUIMouseEvent::Button::Left) {
				_setIsPressed(false);

				axUIButtonEvent outEvent(this);
				outEvent.mouseEvent = &ev;

				if (_isToggleButton) {
					if (localRect().containsPoint(ev.pos)) {
						setIsDown(!_isDown);
						onClick(outEvent);
					}

				} else {
					if (localRect().containsPoint(ev.pos)) {
						onClick(outEvent);
					}
				}
			}
		}break;
	}
}

#if 0
#pragma mark ========== axUIButton ===========================
#endif

axUIButton::axUIButton(axUIView* parent, axStrView labelText)
	: Base(parent)
	, label(this)
{
	setBorderColor(0,0,0,1);
	layout.setFitContentW();
	layout.setFitContentH();
	layout.setMinW(10);
	label.setText(labelText);
}

void axUIButton::onRender(axRenderRequest& req) {
	Color color(0.3f, 0.3f, 0.3f);

    if (isPressed()) {
        color.set(0, 0, 0.1f);

    } else if (isDown()) {
        color.set(0, 0, 0.3f);
    }

    req.ui.drawRect(AX_LOC, localRect(), color);
}
