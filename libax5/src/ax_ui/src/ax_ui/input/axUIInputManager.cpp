#include "axUIInputManager.h"
#include "../base/axUIApp.h"

axUIInputManager* axUIInputManager::s_instance() {
	static axGlobalSingleton<axUIInputManager> s;
	return s.ptr();
}

#if 0
#pragma mark ------- Keyboard -----------------
#endif

void axUIInputManager::_setFocus(axUIView* view) {
	if (_focusView == view)
		return;

	if (_focusView) {
		_focusView->_hasFocus = false;
		_focusView->onSetFocus(false);
		_focusView->setNeedToRender();
	}

	_focusView = view;
//	AX_LOG("setFocus = {?}", _focusView);

	if (_focusView) {
		_focusView->_hasFocus = true;
		_focusView->onSetFocus(true);
		_focusView->setNeedToRender();
	}
}

void axUIInputManager::handleNativeUIKeyEvent(axUIView* inView, axNativeUIKeyEvent& inEvent) {
	AX_ZoneScoped;

	// using Type    = axNativeUIKeyEvent::Type;
	// using KeyCode = axNativeUIKeyEvent::KeyCode;

	axUIKeyEvent ev;

	ev.type		= inEvent.type;
	ev.key		= inEvent.key;
	ev.modifier	= inEvent.modifier;
	ev.text		= inEvent.text;
	ev.time		= inEvent.time;

	auto* app = axUIApp::s_instance();
	if (app->handleKeyboardShortcut(ev)) {
		// handled by app shortcut
	} else if (_focusView) {
		_focusView->handleKeyEvent(ev);
	}
}

#if 0
#pragma mark ------- Mouse or Touch ----------------------
#endif

void axUIInputManager::setMouseCapture(axUIView* view) {
	_mouse._setCapture(view);
}

void axUIInputManager::setMouseContextMenu(axUIView* view) {
	_mouse._contextMenu = view;
}

void axUIInputManager::beginDragAndDrop(axUIView* view) {
	_mouse.beginDragAndDrop(view);
}

void axUIInputManager::handleNativeUIMouseEvent(axUIView* inView, axNativeUIMouseEvent& nativeEvent) {
	_mouse.handleNativeUIMouseEvent(inView, nativeEvent);
}

axUIView* axUIInputManager::MouseOrTouchHandler::_hitTest(axUIView* view, axUIMouseEvent& inEvent) {
	if (!view)
		return nullptr;

	AX_ZoneScoped;

	axUIHitTest ht(inEvent);
	ht.worldPos = _worldPos;
	ht.dragAndDropSource = _dragAndDropSource;

	if (!view->hitTest(ht))
		return nullptr;

	return ht.result.view;
}

void axUIInputManager::MouseOrTouchHandler::_clickEvent(axUIView* view, axUIMouseEvent& ev) {
	//click event (down/up within radius)
	const auto clickTime = axHiResTime::s_seconds(0.35);
	const float	clickRadius = 5;

	if (ev.type != Type::Up || !ax_enum(_pressedButtons).isZero()) return;
	if (_lastDownButton != ev.button) return;
//	if (_captureView != view) return;

	auto deltaTime = ev.time - _lastDownButtonTime;
	if (deltaTime > clickTime) return;

	if (ev.dragDeltaPos.lengthGreaterThan(clickRadius)) return;

	axUIMouseEvent clickEvent = ev;
	clickEvent.type = Type::Click;
	handleMouseEvent(view, clickEvent);
}

void axUIInputManager::MouseOrTouchHandler::handleNativeUIMouseEvent(axUIView* view, axNativeUIMouseEvent& nativeEvent) {
	if (!view) return;

	AX_ZoneScoped;

	_worldPos    = ax_round(nativeEvent.worldPos);

//	_wrapMouseToOppositeSideOfScreen();

	//_buttonState = nativeEvent.buttonState;

	if (nativeEvent.type == Type::Down) {
		ax_enum(_pressedButtons).setFlags(nativeEvent.button);
	} else 	if (nativeEvent.type == Type::Up) {
		ax_enum(_pressedButtons).unsetFlags(nativeEvent.button);
	}

	if (nativeEvent.type == Type::Down && ax_enum(_lastPressedButtons).isZero()) {
		_dragStartWorldPos = _worldPos;
	}

	axUIMouseEvent	ev;
	ev.type			= nativeEvent.type;
	ev.time			= nativeEvent.time;
	ev.modifier		= nativeEvent.modifier;

	ev.button  = nativeEvent.button;
	ev.pressedButtons = _pressedButtons;

	ev.wheelDelta	= nativeEvent.wheelDelta;

	ev.worldPos		= _worldPos;
	ev.deltaPos		= _worldPos - _lastWorldPos;

	ev.dragStartWorldPos = _dragStartWorldPos;
	ev.dragDeltaPos		 = _worldPos - _dragStartWorldPos;

	ev.dragAndDropSource = _dragAndDropSource;

	if (ev.type == Type::Down) {
		_lastDownButton     = ev.button;
		_lastDownButtonTime = ev.time;
	} else {
		ev.pressedTime = ev.time - _lastDownButtonTime;
	}

	//--------
	handleMouseEvent(view, ev);
	_clickEvent(view, ev);
	//---------

	_lastWorldPos       = _worldPos;
	_lastPressedButtons = _pressedButtons;
}

void axUIInputManager::MouseOrTouchHandler::_wrapMouseToOppositeSideOfScreen() {
	if (!_captureView) return;

	const float d = 2;

	auto screen = _captureView->uiScreen();
	auto wpos = _worldPos;
	auto rc = screen.worldRect().expand(-d);

	bool changed = false;

	if (wpos.x < rc.xMin()) {
		wpos.x = rc.xMax() - d;
		changed = true;
	} else if (wpos.x > rc.xMax()) {
		wpos.x = rc.xMin() + d;
		changed = true;
	}

	if (wpos.y < rc.yMin()) {
		wpos.y = rc.yMax() - d;
		changed = true;
	} else if (wpos.y > rc.yMax()) {
		wpos.y = rc.yMin() + d;
		changed = true;
	}

	if (!changed) return;

	if (!axUIApp::s_instance()->setCursorPos(wpos))
		return;

	_worldPos = wpos;
	_lastWorldPos = wpos;
}

void axUIInputManager::MouseOrTouchHandler::handleMouseEvent_dragAndDropSource(axUIView* inView, axUIMouseEvent& inEvent) {
	if (inEvent.type == Type::Move) {
		axUIMouseEvent ev(inEvent);
		ev.type = Type::DragAndDropDragging;
		_dragAndDropSource->handleMouseEvent(ev);
	}

	// hover
	axUIMouseEvent hoverEvent(inEvent);
	hoverEvent.type = Type::DragAndDropQueryAcceptSource;
	auto target = _hitTest(inView, hoverEvent);
	if (target) {
//		AX_DUMP_VAR(target);

		target->handleMouseEvent(hoverEvent);

		if (!hoverEvent.result.dragAndDropAcceptSource) {
			target = nullptr;
		}
	}

	if (target) {
		_setHoverView(target, inEvent);
		_setDragAndDropTarget(target, inEvent);

		if (inEvent.type == Type::Up) {
			axUIMouseEvent ev(inEvent);
			ev.type = Type::DragAndDropEnd;
			ev.dragAndDropSource = _dragAndDropSource;
			ev.dragAndDropTarget = _dragAndDropTarget;

			// target can be null after _setDragAndDropTarget() send event to axUIView
			if (target) {
				target->handleMouseEvent(ev);
			}

			//if (_dragAndDropSource) {
			//	_dragAndDropSource->handleMouseEvent(ev);
			//}

			_setDragAndDropSource(nullptr, inEvent);
			_setDragAndDropTarget(nullptr, inEvent);
		}
	} else {
		_setHoverView(nullptr, inEvent);
		_setDragAndDropTarget(nullptr, inEvent);
	}
}

void axUIInputManager::MouseOrTouchHandler::handleMouseEvent(axUIView* inView, axUIMouseEvent& inEvent) {
	if (!inView) return;

#if 1 // debug
	{
		if (inEvent.type == Type::Down) {
			if (inEvent.modifier == Modifier::Atl) {
				AX_LOG("===== debug hit test =====");
				auto hitTestDebug = _hitTest(inView, inEvent);
				//AX_DUMP_VAR(hitTestDebug);
				if (hitTestDebug) {
					if (hitTestDebug->parent()) {
						AX_DUMP_VAR(hitTestDebug->parent());
					}
					hitTestDebug->dump(0);
				}
			}
		}
	}
#endif
	axLink<axUIView> outView;

	if (_dragAndDropSource) {
		handleMouseEvent_dragAndDropSource(inView, inEvent);

	} else if (_captureView) {
		outView = _captureView;
		if (inEvent.type == Type::Move) {
			inEvent.type = Type::Dragging;
		}

	} else {
		outView = _hitTest(inView, inEvent);
		_setHoverView(outView, inEvent);

		if (inEvent.type == Type::Down) {
			_setCapture(outView, inEvent);
		}
	}

	if (outView) {
		axUIMouseEvent ev(inEvent);
		outView->handleMouseEvent(ev);
	}

	if (_contextMenu) {
		axUIMouseEvent ev(inEvent);
		_contextMenu->handleMouseEvent(ev);
	}

	// in case no button is down but missed 'Up' event
	// for example mouse move away from application window and mouse up
	if (ax_enum(inEvent.pressedButtons).isZero()) {
		_setCapture(nullptr, inEvent);
		_setDragAndDropSource(nullptr, inEvent);
		_setDragAndDropTarget(nullptr, inEvent);
	}
}

void axUIInputManager::MouseOrTouchHandler::beginDragAndDrop(axUIView* view) {
	axUIMouseEvent ev;
	ev.worldPos				= _worldPos;
	ev.deltaPos				= ev.worldPos - _lastWorldPos;

	ev.dragStartWorldPos	= _dragStartWorldPos;
	ev.dragDeltaPos			= _worldPos - _dragStartWorldPos;

	ev.pressedButtons		= _pressedButtons;
	ev.modifier				= _modifier;
	
	_setDragAndDropSource(view,    ev);
	_setDragAndDropTarget(nullptr, ev);
}

void axUIInputManager::MouseOrTouchHandler::_setHoverView(axUIView* view, axUIMouseEvent& inEvent) {
	if (_hoverView == view)
		return;

	if (_hoverView) {
		axUIMouseEvent outEvent(inEvent);
		outEvent.type = Type::Leave;
		_hoverView->handleMouseEvent(outEvent);
		_hoverView->setNeedToRender();
	}

	//AX_LOG("_setHoverView {?}", view);
	_hoverView = view;

	if (view) {
		axUIMouseEvent outEvent(inEvent);
		outEvent.type = Type::Enter;
		_hoverView->handleMouseEvent(outEvent);
		_hoverView->setNeedToRender();
	}
}

axUIMouseEvent axUIInputManager::MouseOrTouchHandler::makeEvent() {
	axUIMouseEvent ev;
	ev.worldPos = _worldPos;
	ev.modifier = _modifier;
	ev.pressedButtons = _pressedButtons;
	ev.time = Time::s_now();
	ev.pressedTime = _lastDownButtonTime - ev.time;
	return ev;
}

void axUIInputManager::MouseOrTouchHandler::_setCapture(axUIView* view) {
	auto ev = makeEvent();
	_setCapture(view, ev);
}

void axUIInputManager::MouseOrTouchHandler::_setCapture(axUIView* view, axUIMouseEvent& inEvent) {
	if (_captureView == view)
		return;

	if (_captureView) {
		axUIMouseEvent ev = inEvent;
		ev.type = Type::DragEnd;
		_captureView->handleMouseEvent(ev);
	}

	_captureView = view;
	//AX_DUMP_VAR(_captureView);

	if (_captureView) {
		axUIMouseEvent ev = inEvent;
		ev.type = Type::DragBegin;
		_captureView->handleMouseEvent(ev);
	}
}

void axUIInputManager::MouseOrTouchHandler::_setDragAndDropSource(axUIView* view, axUIMouseEvent& inEvent) {
	if (_dragAndDropSource == view)
		return;

	if (_dragAndDropSource) {
		axUIMouseEvent ev = inEvent;
		ev.type = Type::DragAndDropEnd;
		_dragAndDropSource->handleMouseEvent(ev);
	}

	_dragAndDropSource = view;

	if (view) {
		axUIMouseEvent ev = inEvent;
		ev.type = Type::DragAndDropBegin;
		view->handleMouseEvent(ev);
	}

//	AX_DUMP_VAR(_dragAndDropSource);
}

void axUIInputManager::MouseOrTouchHandler::_setDragAndDropTarget(axUIView* view, axUIMouseEvent& inEvent) {
	if (_dragAndDropTarget == view)
		return;

	_dragAndDropTarget = view;
//	AX_DUMP_VAR(_dragAndDropTarget);
}
