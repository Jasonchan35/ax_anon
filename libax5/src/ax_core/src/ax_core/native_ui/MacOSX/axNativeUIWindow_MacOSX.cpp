#if AX_OS_MACOSX

#include "axNativeUIEventUtil_MacOSX.h"
#include "axNativeUIWindow_MacOSX.h"
#include "../axNativeUIApp.h"

@implementation axNativeUIWindow_MacOSX_NSWin
- (BOOL)canBecomeMainWindow {
	return true;
}

- (void)setCpp:(axNativeUIWindow_MacOSX*)cpp {
	_cpp = cpp;
}

- (void)onCustomDestroyWindow {
	ax_delete(_cpp);
	return;
}

@end

#pragma mark -------------------------------------------------

@interface axNativeUIWindow_MacOSX_NSWinDelegate : NSObject< NSWindowDelegate > {
	axNativeUIWindow_MacOSX* _cpp;
	NSWindow* _nswin;
}
- (instancetype)init:(NSWindow*)nswin Cpp:(axNativeUIWindow_MacOSX*)cpp;
@end

@implementation axNativeUIWindow_MacOSX_NSWinDelegate
- (instancetype)init:(NSWindow*)nswin Cpp:(axNativeUIWindow_MacOSX*)cpp {
	self = [super init];
	_nswin = nswin;
	_cpp = cpp;
	return self;
}
- (void)onDestroy {
	_cpp = nullptr;
	_nswin = nil;
}
- (BOOL)windowShouldClose:(id)sender {
	if (!_cpp) return false;
	_cpp->onCloseButton();
	return false;
}
- (void)windowWillClose:(NSNotification *)notification {
//	AX_ASSERT(_cpp == nullptr);
}
- (void)windowDidBecomeMain:(NSNotification *)notification {
	if (_cpp) {
//		_cpp->onSetWorldPos(_cpp->getWorldRect().pos); // the getWorldRect is incorrect until here, because of [self screen]
		_cpp->onActive(true);
	}
}
- (void)windowDidResignMain:(NSNotification *)notification {
	if (_cpp) _cpp->onActive(false);
}

- (void)windowDidMove:(NSNotification *)notification {
	if (_cpp) _cpp->onSetWorldPos(_cpp->getWorldRect().pos);
}
- (void)windowDidResize:(NSNotification *)notification {
	if (!_cpp) return;
	auto rc = axRect2f([_cpp->_nswin frame]);
	_cpp->onSetSize(rc.size);
	
	// postLayout event is blocked by live resize, so call updateLayout directly instead
	auto* app = axNativeUIApp_Base::s_instance();
	app->updateLayout();
}
- (void)windowDidEndLiveResize:(NSNotification *)notification {
	if (!_cpp) return;
	auto rc = axRect2f([_cpp->_nswin frame]);
	_cpp->onSetSize(rc.size);
}
- (void)windowDidChangeScreen:(NSNotification *)notification {
	// in case Window changed screen with different scale factor
	axNativeUIApp::s_instance()->setNeedToRender();
}
@end


#pragma mark ----------------------------------------------------------
@interface axNativeUIWindow_MacOSX_ContentView : NSView {
	axNativeUIWindow_MacOSX* _cpp;
}
- (instancetype)init:(axNativeUIWindow_MacOSX*)cpp;
@end

@implementation axNativeUIWindow_MacOSX_ContentView
- (instancetype)init:(axNativeUIWindow_MacOSX*)cpp {
	self = [super initWithFrame:axRect2f(0,0,0,0)];
	_cpp = cpp;
	return self;
}
@end

#pragma mark ----------------------------------------------------------

axNativeUIScreen axNativeUIWindow_MacOSX::screen() { return [_nswin screen]; }
NSWindow*	axNativeUIWindow_MacOSX::nsWin		() { return _nswin; }
NSView*		axNativeUIWindow_MacOSX::contentView() { return _contentView; }

axNativeUIWindow_MacOSX::axNativeUIWindow_MacOSX(CreateDesc& desc)
	: Base(desc)
{
	NSWindowStyleMask style = 0;
	
	if (desc.closeButton) style |= NSWindowStyleMaskClosable;
	if (desc.minButton  ) style |= NSWindowStyleMaskMiniaturizable;
	if (desc.resizable  ) style |= NSWindowStyleMaskResizable;
	
	switch (desc.type) {
		case CreateDesc::Type::NormalWindow: {
			style |= NSWindowStyleMaskTitled;
		}break;
		case CreateDesc::Type::PopupWindow: {
			
		}break;
		case CreateDesc::Type::ToolWindow: {
			style |= NSWindowStyleMaskUtilityWindow;
		}break;
		default: AX_ASSERT(false); break;
	}

	auto desktopRect = axNativeUIApp::s_getVirtualDesktopRect();
	auto rect = desc.rect.flipYWithinContainer(desktopRect.h);

	_nswin.ref([[axNativeUIWindow_MacOSX_NSWin alloc]
					initWithContentRect: rect
					styleMask: style
					backing: NSBackingStoreBuffered
					defer: true]);
	if (!_nswin) throw axError_Undefined(AX_LOC);
	
	[_nswin setCpp:this];

	_delegate.ref([[axNativeUIWindow_MacOSX_NSWinDelegate alloc]
						init:_nswin Cpp:this]);
	if (!_delegate) throw axError_Undefined(AX_LOC);
	
	[_nswin setDelegate:_delegate];
	[_nswin setReleasedWhenClosed:false];
	[_nswin setAcceptsMouseMovedEvents:true];	//mouse move
	[_nswin setIgnoresMouseEvents:false];		//other mouse events
	[_nswin setShowsResizeIndicator:desc.resizable];
	[_nswin setLevel: NSNormalWindowLevel];
	[_nswin makeMainWindow];
	
	{
		axRect2f tmp(0, 0, 400, 400);
		auto rc = axRect2f([_nswin contentRectForFrameRect: tmp.to_CGRect()]);
		auto padding = tmp - rc;
		ax_swap(padding.top, padding.bottom);
		setPadding(padding);
	}

	{
		auto rc = getWorldRect();
		setWorldPos(rc.pos);
		onSetSize(rc.size);
	}
	
	_contentView.ref([[axNativeUIWindow_MacOSX_ContentView alloc] init:this]);
	if (!_contentView) throw axError_Undefined(AX_LOC);
	[_contentView setAutoresizesSubviews:false];
	
	[_nswin setContentView:_contentView];
	setVisible(desc.visible);
}

axNativeUIWindow_MacOSX::~axNativeUIWindow_MacOSX() {
	if(_nswin) {
		[_delegate onDestroy];
		[_nswin close];
	}
}

void axNativeUIWindow_MacOSX::doCloseWindow() {
	[_nswin performClose:nil];
}

void axNativeUIWindow_MacOSX::doDestroyWindow() {
	
	auto ev = [NSEvent	otherEventWithType:NSEventTypeApplicationDefined
						location:{0,0}
						modifierFlags:0
						timestamp:0
						windowNumber: [_nswin windowNumber]
						context: nullptr
						subtype: ax_enum_int(axNativeUI_CustomNSEventSubType::CustomDestroyWindow)
						data1:0
						data2:0];
	
	[_delegate onDestroy];
	[_nswin postEvent:ev atStart:true];
}

axRect2f axNativeUIWindow_MacOSX::getWorldRect() const {
	auto rc = axRect2f([_nswin frame]);
	rc.pos.set(0,0);
	rc = axRect2f([_nswin convertRectToScreen:rc]);

	auto screenFrame = axRect2f([_nswin screen].frame);
	rc.pos += screenFrame.pos;

	auto desktopRect = axNativeUIApp::s_getVirtualDesktopRect();
	rc = rc.flipYWithinContainer(desktopRect.h);
	return rc;
}

void axNativeUIWindow_MacOSX::onSetNativeWorldPos(const axVec2f& v) {
	auto rc = axRect2f([_nswin frame]);
	rc.pos = v;

	auto desktopRect = axNativeUIApp::s_getVirtualDesktopRect();
	rc = rc.flipYWithinContainer(desktopRect.h);

	auto screenFrame = axRect2f([_nswin screen].frame);
	rc.pos -= screenFrame.pos - axRect2f([_nswin frame]).pos;
	auto finalRc = [_nswin convertRectFromScreen:rc];

	[_nswin setFrame:finalRc display:true];
}

void axNativeUIWindow_MacOSX::onSetNativeSize(const axVec2f& v) {
	auto rc =[_nswin frame];
	rc.size = v;
	[_nswin setFrame:rc display:true];
}

void axNativeUIWindow_MacOSX::onSetNativeActive(bool b) {
	if (b) {
		[_nswin orderFrontRegardless];
		[_nswin makeMainWindow];
		[_nswin makeKeyWindow]; // for keyboard input
	}else{
		AX_ASSERT_NOT_IMPLEMENTED();
	}
}

void axNativeUIWindow_MacOSX::onSetNativeVisible(bool b) {
	[_nswin setIsVisible: b];
}

void axNativeUIWindow_MacOSX::onSetNativeWindowTitle(axStrView title) {
	[_nswin setTitle: ax_toNSString(title)];
}

void axNativeUIWindow_MacOSX::_handleNativeUIMouseEvent(NSEvent* inEvent) {
	if (!_nswin) return;

	using Button = axNativeUIMouseEventButton;

	axNativeUIMouseEvent ev;
	ev.time.setSecondsInFloat64(inEvent.timestamp);
	ev.modifier = axNativeUIEventUtil_MacOSX::convertModifierFlags([inEvent modifierFlags]);

#if 0
	{
		auto pos = axVec2f([inEvent locationInWindow]);
		auto rect = worldRect();
		pos.y = rect.h - pos.y - 1;
		pos += rect.pos;
		ev.worldPos = pos;
	}
#else
	{
//		auto pos = [_nswin convertPointToScreen:[inEvent locationInWindow]];
//		auto sf = [_nswin screen].frame;
//		pos.y = sf.size.height - pos.y - 1;
		auto pos = axVec2f([inEvent locationInWindow]);
		pos.y = worldRect().h - pos.y - 1;
		pos += worldRect().pos;

		// tmp = [_nswin convertRectToScreen:[_nswin frame]];
		//AX_DUMP_VAR(ev.worldPos, getWorldRect(), worldRect(), pos);
		ev.worldPos.setByCast(pos);
	}
#endif

	switch (inEvent.type) {
		case NSEventTypeOtherMouseUp:
		case NSEventTypeRightMouseUp:
		case NSEventTypeLeftMouseUp: {
			ev.type = axNativeUIMouseEventType::Up;
		}break;
	
		case NSEventTypeOtherMouseDown:
		case NSEventTypeRightMouseDown:
		case NSEventTypeLeftMouseDown: {
			[_nswin makeMainWindow];
			[_nswin makeKeyWindow];
			ev.type = axNativeUIMouseEventType::Down;
		}break;
	
		case NSEventTypeRightMouseDragged:
		case NSEventTypeLeftMouseDragged:
		case NSEventTypeOtherMouseDragged:
		case NSEventTypeMouseMoved: {
			ev.type = axNativeUIMouseEventType::Move;
		}break;
	
		case NSEventTypeScrollWheel: {
			ev.type = axNativeUIMouseEventType::Wheel;
			const float s = 12.0f;
			ev.wheelDelta.x = static_cast<float>(inEvent.scrollingDeltaX) * s;
			ev.wheelDelta.y = static_cast<float>(inEvent.scrollingDeltaY) * s;
		}break;
	
		default: {
			AX_ASSERT(false);
			return;
		}break;
	}
	
	switch (inEvent.buttonNumber) {
		case 0:  ev.button = Button::Left;   break;
		case 1:  ev.button = Button::Right;  break;
		case 2:  ev.button = Button::Middle; break;
		case 3:  ev.button = Button::Button4; break;
		case 4:  ev.button = Button::Button5; break;
	}
	/* supposed to work on OSX 10.6, but can't compile with no reason
	auto buttonMask = inEvent.pressedMouseButtons;
	if (buttonMask) {
		if (buttonMask & (1<<0)) ev.buttonState.setFlags(Button::Left);
		if (buttonMask & (1<<1)) ev.buttonState.setFlags(Button::Right);
		if (buttonMask & (1<<2)) ev.buttonState.setFlags(Button::Middle);
		if (buttonMask & (1<<3)) ev.buttonState.setFlags(Button::Button4);
		if (buttonMask & (1<<4)) ev.buttonState.setFlags(Button::Button5);
	}
	*/
	
	_onNativeUIMouseEvent(ev);
}

void axNativeUIWindow_MacOSX::_handleNativeUIKeyEvent(NSEvent* inEvent) {
	using Type = axNativeUIKeyEventType;
	using KeyCode = axNativeUIKeyCode;
	
	axNativeUIKeyEvent ev;
	ev.time.setSecondsInFloat64(inEvent.timestamp);
	ev.modifier = EventUtil::convertModifierFlags([inEvent modifierFlags]);

	ev.key = EventUtil::convertKey(inEvent);
	ev.isRepeat = inEvent.ARepeat;

	switch (inEvent.type) {
		case NSEventTypeKeyDown: {
		//---- send char event ----
			ev.type = Type::Char;
			switch (ev.key) {
				case KeyCode::Backspace:	ev.text.setUtf("\b"); break;
				case KeyCode::Enter:		ev.text.setUtf("\n"); break;
				case KeyCode::None:			ev.text.setUtf(axStrView_make(inEvent.characters)); break;
			}
			if (ev.text.size()) {
				_onNativeUIKeyEvent(ev);
				ev.text.clear();
			}
		//--------
			ev.type = Type::Down;
			// if (ev.isRepeat) return; // don't send repeat down
		}break;
		
		case NSEventTypeKeyUp: {
			ev.type = Type::Up;
		}break;
		
		default: {
			AX_ASSERT(false);
			return;
		}break;
	}
	
	_onNativeUIKeyEvent(ev);
}
 
#endif //AX_OS_MACOSX
