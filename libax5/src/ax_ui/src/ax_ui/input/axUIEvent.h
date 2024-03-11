#pragma once

#include <ax_core/math/axRect2.h>
#include <ax_core/other/axEnum.h>

using axUIMouseEventType	= axNativeUIMouseEventType;
using axUIKeyEventType		= axNativeUIKeyEventType;
using axUIEventModifier		= axNativeUIEventModifier;
using axUIMouseEventButton	= axNativeUIMouseEventButton;
using axUIKeyCode			= axNativeUIKeyCode;
using axUIEventTime			= axNativeUIEventTime;

class axUIMouseEvent {
public:
	using Type		= axUIMouseEventType;
	using Button	= axUIMouseEventButton;
	using Modifier	= axUIEventModifier;
	using Time		= axUIEventTime;

	Type		type;
	Button		button;
	Button		pressedButtons;
	Modifier	modifier;
	axVec2f		worldPos{0,0};
	axVec2f		wheelDelta{0,0};

	axVec2f		pos{0,0};
	axVec2f		deltaPos{0,0};
	axVec2f		dragStartWorldPos{0,0};
	axVec2f		dragDeltaPos{0,0};

	axLink<axUIView>	dragAndDropSource;
	axLink<axUIView>	dragAndDropTarget;

	Time		time;
	Time		pressedTime = Time::s_zero();

	struct Result {
		bool dragAndDropAcceptSource = false;
	};
	Result result;

	void onFormat(axFormat& f) const;

	bool isDragging() const { return type == Type::Dragging; }

	bool isLeftDown		() const { return type == Type::Down  && button == Button::Left;	}
	bool isMiddleDown	() const { return type == Type::Down  && button == Button::Middle;	}
	bool isRightDown	() const { return type == Type::Down  && button == Button::Right;	}

	bool isLeftUp		() const { return type == Type::Up    && button == Button::Left;	}
	bool isMiddleUp		() const { return type == Type::Up    && button == Button::Middle;	}
	bool isRightUp		() const { return type == Type::Up    && button == Button::Right;	}

	bool isLeftClick	() const { return type == Type::Click && button == Button::Left;	}
	bool isMiddleClick() const { return type == Type::Click && button == Button::Middle;}
	bool isRightClick	() const { return type == Type::Click && button == Button::Right;	}

	bool isShiftDown	() const { return modifier == Modifier::Shift;	}
	bool isCtrlDown		() const { return modifier == Modifier::Ctrl;	}
	bool isAtlDown		() const { return modifier == Modifier::Atl;	}
	bool isCmdDown		() const { return modifier == Modifier::Cmd;	}
	bool isFnDown		() const { return modifier == Modifier::Fn;		}
};

class axUIShortcutKey {
	using This = axUIShortcutKey;
public:
	using KeyCode	= axUIKeyCode;
	using Modifier	= axUIEventModifier;

	axUIShortcutKey() = default;
	axUIShortcutKey(Modifier mod, KeyCode key_) : modifier(mod), key(key_) {}

	bool operator==(const This& r) const { return modifier == r.modifier && key == r.key; }
	bool operator!=(const This& r) const { return modifier != r.modifier || key != r.key; }
	bool operator< (const This& r) const { return modifier <  r.modifier || key <  r.key; }

	axHash onGetHash() const { return axHash(static_cast<axHash::Value>(ax_enum_int(modifier)) << 32 | ax_enum_int(key)); }

	Modifier	modifier = Modifier::None;
	KeyCode		key		 = KeyCode::None;
};

class axUIKeyEvent {
public:
	using Type		= axUIKeyEventType;
	using KeyCode	= axUIKeyCode;
	using Modifier	= axUIEventModifier;
	using Time		= axUIEventTime;

	bool isDown	() const { return type == Type::Down; }
	bool isUp	() const { return type == Type::Up;	  }
	bool isChar	() const { return type == Type::Char; }

	axUIShortcutKey	shortcutKey() const { return axUIShortcutKey(modifier, key); }
	void onFormat(axFormat& f) const;

	Type			type;
	Modifier		modifier;
	KeyCode			key;
	axStringU		text;
	Time			time;
};

class axUIHitTest {
public:
	using Type		= axUIMouseEventType;
	using Button	= axUIMouseEventButton;
	using Modifier	= axUIEventModifier;

	axUIHitTest(const axUIMouseEvent& inEvent_) : inEvent(inEvent_) {}

	const axUIMouseEvent& inEvent;

	axVec2f				worldPos {0,0};
	axVec2f				pos {0,0};

	axLink<axUIView>	dragAndDropSource;

	struct Result {
		axUIView* view = nullptr;
	};
	Result result;
};

class axUIRenderEventHandler : public axRenderContext::EventHandler {
	using Base = axRenderContext::EventHandler;
public:
	virtual void onRender(axRenderRequest& req) override;
	virtual void onUpdateGpuResources(axRenderCommandDispatcher& dispatcher) override;
};
