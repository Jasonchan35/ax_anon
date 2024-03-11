#pragma once

#include <ax_core/math/axRect2.h>
#include <ax_core/other/axEnum.h>
#include <ax_core/time/axTimestamp.h>

#define axNativeUIMouseEventType_EnumList(E) \
	E(None,) \
	E(Up,) \
	E(Down,) \
	E(Click,) \
	E(DoubleClick,) \
	E(Move,) \
	E(DragBegin,) \
	E(Dragging,) \
	E(DragEnd,) \
	E(Wheel,) \
	E(Enter,) \
	E(Leave,) \
	E(DragAndDropBegin,) \
	E(DragAndDropDragging,) \
	E(DragAndDropQueryAcceptSource,) \
	E(DragAndDropEnd,) \
//----
ax_ENUM_CLASS(axNativeUIMouseEventType, axUInt8)

#define axNativeUIMouseEventButton_EnumList(E) \
	E(None,    = 0     ) \
	E(Left,    = 1 << 0) \
	E(Middle,  = 1 << 1) \
	E(Right,   = 1 << 2) \
	E(Button4, = 1 << 4) \
	E(Button5, = 1 << 5) \
//----
ax_ENUM_FLAGS_CLASS(axNativeUIMouseEventButton, axUInt8)

#define axNativeUIKeyEventType_EnumList(E) \
	E(None,) \
	E(Up,) \
	E(Down,) \
	E(Char,) \
//----
ax_ENUM_CLASS(axNativeUIKeyEventType, axUInt8)

#define axNativeUIKeyCode_EnumList(EV) \
	EV(None,) \
	\
	EV(LMouseButton,) \
	EV(MMouseButton,) \
	EV(RMouseButton,) \
	EV(OtherMouseButton,) \
	\
	EV(Backspace,) \
	EV(Tab,) \
	EV(Clear,) \
	EV(Enter,) \
	EV(Shift,) \
	EV(Ctrl,) \
	EV(Alt,) \
	EV(Pause,) \
	EV(CapsLock,) \
	EV(IME_Kana,) \
	EV(IME_Hangul,) \
	EV(IME_Junja,) \
	EV(IME_Final,) \
	EV(IME_Hanja,) \
	EV(IME_Kanji,) \
	EV(Escape,) \
	EV(IME_Convert,) \
	EV(IME_NonConvert,) \
	EV(IME_Accept,) \
	EV(IME_ModeChange,) \
	\
	EV(Space,) \
	EV(Comma,) \
	EV(Equals,) \
	EV(Minus,) \
	EV(SemiColon,) \
	EV(BackQuote,) \
	EV(Pipe,) \
	EV(Quotes,) \
	EV(Slash,) \
	EV(BackSlash,) \
	EV(Period,) \
	EV(ProcessKey,) \
	\
	EV(LBracket,) \
	EV(RBracket,) \
	\
	EV(PageUp,) \
	EV(PageDown,) \
	EV(Begin,) \
	EV(End,) \
	EV(Home,) \
	EV(LeftArrow,) \
	EV(UpArrow,) \
	EV(RightArrow,) \
	EV(DownArrow,) \
	\
	EV(User,) \
	EV(System,) \
	EV(SysReq,) \
	EV(ModeSwitch,) \
	EV(ScriptSwitch,) \
	EV(Break,) \
	EV(Cancel,) \
	EV(Undo,) \
	EV(Redo,) \
	EV(Find,) \
	EV(Menu,) \
	EV(Reset,) \
	EV(Stop,) \
	\
	EV(Prev,) \
	EV(Next,) \
	\
	EV(Select,) \
	EV(Print,) \
	EV(Execute,) \
	EV(PrintScreen,) \
	EV(Insert,) \
	EV(Delete,) \
	EV(Help,) \
	EV(LWin,) \
	EV(RWin,) \
	EV(Apps,) \
	EV(Sleep,) \
	\
	EV(NumLock,) \
	EV(ScrollLock,) \
	EV(Separator,) \
	\
	EV(NumPadAdd,) \
	EV(NumPadSub,) \
	EV(NumPadMul,) \
	EV(NumPadDiv,) \
	\
	EV(NumPadDecimal,) \
	EV(NumPadEqual,) \
	\
	EV(NumPadEnter,) \
	EV(NumPadInsert,) \
	EV(NumPadDelete,) \
	\
	EV(NumPadUp,) \
	EV(NumPadRight,) \
	EV(NumPadDown,) \
	EV(NumPadLeft,) \
	EV(NumPadPageUp,) \
	EV(NumPadPageDown,) \
	\
	EV(NumPadBegin,) \
	EV(NumPadEnd,) \
	EV(NumPadHome,) \
	EV(NumPadPrev,) \
	EV(NumPadNext,) \
	\
	EV(LShift,) \
	EV(RShift,) \
	EV(LCtrl,) \
	EV(RCtrl,) \
	EV(LAlt,) \
	EV(RAlt,) \
	\
	EV(BrowserBack,) \
	EV(BrowserForward,) \
	EV(BrowserRefresh,) \
	EV(BrowserStop,) \
	EV(BrowserSearch,) \
	EV(BrowserFavorites,) \
	EV(BrowserHome,) \
	\
	EV(VolumeMute,) \
	EV(VolumeDown,) \
	EV(VolumeUp,) \
	\
	EV(MediaNextTrack,) \
	EV(MediaPrevTrack,) \
	EV(MediaStop,) \
	EV(MediaPlayPause,) \
	\
	EV(LaunchMail,) \
	EV(LaunchMediaSelect,) \
	EV(LaunchMediaApp1,) \
	EV(LaunchMediaApp2,) \
	\
	         EV(F1 ,) EV(F2 ,) EV(F3 ,) EV(F4 ,) EV(F5 ,) EV(F6 ,) EV(F7 ,) EV(F8 ,) EV(F9 ,) \
	EV(F10,) EV(F11,) EV(F12,) EV(F13,) EV(F14,) EV(F15,) EV(F16,) EV(F17,) EV(F18,) EV(F19,) \
	EV(F20,) EV(F21,) EV(F22,) EV(F23,) EV(F24,) EV(F25,) EV(F26,) EV(F27,) EV(F28,) EV(F29,) \
	EV(F30,) EV(F31,) EV(F32,) EV(F33,) EV(F34,) EV(F35,) \
	\
	EV(Digi0,) EV(Digi1,) EV(Digi2,) EV(Digi3,) EV(Digi4,) \
	EV(Digi5,) EV(Digi6,) EV(Digi7,) EV(Digi8,) EV(Digi9,) \
	\
	EV(NumPad0,) EV(NumPad1,) EV(NumPad2,) EV(NumPad3,) EV(NumPad4,) \
	EV(NumPad5,) EV(NumPad6,) EV(NumPad7,) EV(NumPad8,) EV(NumPad9,) \
	\
	EV(A,) EV(B,) EV(C,) EV(D,) EV(E,) \
	EV(F,) EV(G,) EV(H,) EV(I,) EV(J,) \
	EV(K,) EV(L,) EV(M,) EV(N,) EV(O,) \
	EV(P,) EV(Q,) EV(R,) EV(S,) EV(T,) \
	EV(U,) EV(V,) EV(W,) EV(X,) EV(Y,) \
	EV(Z,) \
//----
ax_ENUM_CLASS(axNativeUIKeyCode, axUInt16)

#define axNativeUIEventModifier_EnumList(E) \
	E(None,		= 0     ) \
	E(Shift,	= 1 << 0) \
	E(Ctrl,		= 1 << 1) \
	E(Atl,		= 1 << 2) \
	E(Cmd,		= 1 << 3) \
	E(Fn,		= 2 << 4) \
//----
ax_ENUM_FLAGS_CLASS(axNativeUIEventModifier, axUInt8);

using axNativeUIEventTime = axHiResTime;

class axNativeUIMouseEvent {
public:
	using Type		= axNativeUIMouseEventType;
	using Button	= axNativeUIMouseEventButton;
	using Modifier	= axNativeUIEventModifier;
	using Time		= axNativeUIEventTime;

	void onFormat(axFormat& f) const;

	Type			type = Type::None;
	Button			button = Button::None;
	Modifier		modifier = Modifier::None;
	axVec2f			worldPos{0,0};
	axVec2f			wheelDelta{0,0};
	Time			time;
};

class axNativeUIKeyEvent {
public:
	using Type		= axNativeUIKeyEventType;
	using KeyCode	= axNativeUIKeyCode;
	using Modifier	= axNativeUIEventModifier;
	using Time		= axNativeUIEventTime;

	void onFormat(axFormat& f) const;

	Type			type = Type::None;
	KeyCode			key;
	Modifier		modifier;
	axStringU		text;
	bool			isRepeat = false;
	Time			time;
};

#define  axUICursorType_EnumList(E) \
	E(Unknown,) \
	E(Arrow,) \
	E(ClosedHand,) \
	E(ContextualMenu,) \
	E(CrossHair,) \
	E(DisappearingItem,) \
	E(DragCopy,) \
	E(DragLink,) \
	E(Help,) \
	E(IBeam,) \
	E(IBeamVertical,) \
	E(NotAllowed,) \
	E(OpenHand,) \
	E(PointingHand,) \
	E(ResizeAll,) \
	E(ResizeNESW,) \
	E(ResizeNS,) \
	E(ResizeNWSE,) \
	E(ResizeWE,) \
	E(ShortWait,) \
	E(Wait,) \
//-------
ax_ENUM_CLASS(axUICursorType, axUInt8)


