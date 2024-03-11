#pragma once

#include "axUIEvent.h"

class axUIInputManager : public axObject {
	AX_RTTI_CLASS(axUIInputManager, axObject);
public:
	static This* s_instance();

	void beginDragAndDrop			(axUIView* view);

	void _setFocus(axUIView* view);

	axUIView* focusView() { return _focusView.ptr(); }

	const axVec2f&	mouseWorldPos() const { return _mouse._worldPos; }

	void setMouseCapture(axUIView* view);
	void setMouseContextMenu(axUIView* view);

friend class axUIWindow;
protected:
	void handleNativeUIMouseEvent	(axUIView* inView, axNativeUIMouseEvent& ev);
	void handleNativeUIKeyEvent		(axUIView* inView, axNativeUIKeyEvent&   ev);

private:
	class MouseOrTouchHandler {
	public:
		using Type		= axUIMouseEventType;
		using Button	= axUIMouseEventButton;
		using Modifier	= axUIEventModifier;
		using Time		= axUIEventTime;

		void beginDragAndDrop			(axUIView* view);
		void handleNativeUIMouseEvent	(axUIView* inView, axNativeUIMouseEvent& ev);

		void handleMouseEvent			(axUIView* view, axUIMouseEvent& inEvent);
		void handleMouseEvent_dragAndDropSource	(axUIView* inView, axUIMouseEvent& inEvent);

		void _wrapMouseToOppositeSideOfScreen();
		void _clickEvent(axUIView* view, axUIMouseEvent& ev);

		void _setHoverView				(axUIView* view, axUIMouseEvent& inEvent);
		void _setCapture				(axUIView* view);
		void _setCapture				(axUIView* view, axUIMouseEvent& inEvent);
		void _setDragAndDropSource		(axUIView* view, axUIMouseEvent& inEvent);
		void _setDragAndDropTarget		(axUIView* view, axUIMouseEvent& inEvent);

		axUIMouseEvent		makeEvent();

		axUIView* _hitTest(axUIView* view, axUIMouseEvent& inEvent);

		axVec2f				_worldPos	  {0,0};
		axVec2f				_lastWorldPos {0,0};
		axVec2f				_dragStartWorldPos {0,0};

		Button				_lastDownButton = Button::None;
		Time				_lastDownButtonTime;

		Modifier			_modifier;
		Button				_pressedButtons;
		Button				_lastPressedButtons;

		axLink<axUIView>	_hoverView;
		axLink<axUIView>	_captureView;
		axLink<axUIView>	_contextMenu;
		axLink<axUIView>	_dragAndDropSource;
		axLink<axUIView>	_dragAndDropTarget;
	};
	MouseOrTouchHandler		_mouse;

	axLink<axUIView>	_focusView;
};
