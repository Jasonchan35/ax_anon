#if AX_OS_MACOSX

#include "../axNativeUIApp.h"
#include "../axNativeUIWindow.h"
#include "../../math/axBBox2.h"

@interface axNativeUIApp_MacOSX_NSDelegate : NSObject < NSApplicationDelegate > {}
@end

@implementation axNativeUIApp_MacOSX_NSDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	axNativeUIApp_MacOSX* app = axNativeUIApp_MacOSX::s_instance();
	app->create();
}
- (void)applicationWillTerminate:(NSNotification *)notification {
	axNativeUIApp_MacOSX* app = axNativeUIApp_MacOSX::s_instance();
	app->willQuit();
}
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
	return NSTerminateNow;
}
- (void)applicationDidHide:(NSNotification *)notification {
}
- (void)applicationDidUnhide:(NSNotification *)notification {
}
- (void)applicationDidBecomeActive:(NSNotification *)notification {
}
- (void)applicationWillBecomeActive:(NSNotification *)notification {
}

@end

@interface axNativeUIApp_MacOSX_NSApp : NSApplication
@end

@implementation axNativeUIApp_MacOSX_NSApp
- (void)sendEvent:(NSEvent *)ev {
	// Ref: Cocoa Event Handling Guide
	// NSApplicationDefined is of real use to application programs.
	// It allows the application to generate custom events and insert them into the event queue.
	// Each such event can have a subtype and two additional codes to differentiate it from others.
	// The NSEvent method otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:
	// creates one of these events, and the subtype, data1, and data2 methods return the information specific to these events.

	using SubType = axNativeUI_CustomNSEventSubType;

	if (ev.type == NSEventTypeApplicationDefined) {
		switch (SubType(ev.subtype)) {
			case SubType::CustomDestroyWindow: {
				if (ev.window) {
					auto win = (axNativeUIWindow_MacOSX_NSWin*)ev.window;
					[win onCustomDestroyWindow];
				}
			}break;
			case SubType::CustomAppEvent: {
				axNativeUIApp::s_instance()->handleCustomAppEvent(false);
			}break;
		}
		return;
	}
	[super sendEvent:ev];
}
@end

axNativeUIApp_MacOSX::axNativeUIApp_MacOSX() {
	_delegate.ref([axNativeUIApp_MacOSX_NSDelegate new]);
	if (!_delegate) throw axError_Undefined(AX_LOC);
	
	[axNativeUIApp_MacOSX_NSApp sharedApplication];
	[NSApp setDelegate:_delegate];

	/*
	 st = menuBar.init();		if( !st ) return st;
	 [NSApp setMainMenu:menuBar._os_menu];

	 NSMenu* quitMenu = [[NSMenu alloc] initWithTitle:@""];
	 [quitMenu addItemWithTitle:@"Quit" action:@selector (terminate:)  keyEquivalent:@"q"];

	 NSMenuItem* mi = [menuBar._os_menu addItemWithTitle:@"" action:nil keyEquivalent:@""];
	 [mi setSubmenu: quitMenu ];
	 */
}

axNativeUIApp_MacOSX::~axNativeUIApp_MacOSX() {
}

int axNativeUIApp_MacOSX::onRun() {
	[NSApp run];
	return _returnCode;
}

void axNativeUIApp_MacOSX::quit(int returnCode) {
	_returnCode = returnCode;
	[NSApp stop:nil];
}

axRect2f axNativeUIApp_MacOSX::s_getVirtualDesktopRect() {
	NSArray<NSScreen *>* screenArray = [NSScreen screens];

	axBBox2f bbox;

	auto n = [screenArray count];
	for (NSUInteger i = 0; i < n; i++) {
		auto* s = [screenArray objectAtIndex:i];
		bbox.include(axRect2f([s frame]));
	}

	return bbox.toRect2();
}

bool axNativeUIApp_MacOSX::onRequestNativeCustomAppEvent() {
	auto ev = [NSEvent	otherEventWithType:NSEventTypeApplicationDefined
								 location:{0,0}
							modifierFlags:0
								timestamp:0
							 windowNumber:0
								  context: nullptr
								  subtype: ax_enum_int(axNativeUI_CustomNSEventSubType::CustomAppEvent)
									data1:0
									data2:0];

	[NSApp postEvent:ev atStart:0];
	return true;
}

bool axNativeUIApp_MacOSX::onSetNativeCursor(axUICursorType type) {
	using Type = axUICursorType;
	NSCursor* o = nullptr;
	switch (type) {
		case Type::Arrow: 				o = [NSCursor arrowCursor]; break;
		case Type::ClosedHand:			o = [NSCursor closedHandCursor]; break;
		case Type::ContextualMenu:		o = [NSCursor contextualMenuCursor]; break;
		case Type::CrossHair:			o = [NSCursor crosshairCursor]; break;
		case Type::DisappearingItem:	o = [NSCursor disappearingItemCursor]; break;
		case Type::DragCopy:			o = [NSCursor dragCopyCursor]; break;
		case Type::DragLink:			o = [NSCursor dragLinkCursor]; break;
//		case Type::Help:				o = [NSCursor ]; break;
		case Type::IBeam:				o = [NSCursor IBeamCursor]; break;
		case Type::IBeamVertical:		o = [NSCursor IBeamCursorForVerticalLayout]; break;
		case Type::NotAllowed:			o = [NSCursor operationNotAllowedCursor]; break;
		case Type::OpenHand:			o = [NSCursor openHandCursor]; break;
		case Type::PointingHand:		o = [NSCursor pointingHandCursor]; break;
//		case Type::ResizeAll:			o = [NSCursor ]; 		break;
//		case Type::ResizeNESW:			o = [NSCursor ]; 	break;
		case Type::ResizeNS:			o = [NSCursor resizeUpDownCursor]; 		break;
//		case Type::ResizeNWSE:			o = [NSCursor ]; 	break;
		case Type::ResizeWE:			o = [NSCursor resizeLeftRightCursor]; 	break;
//		case Type::ShortWait:			o = [NSCursor ]; break;
//		case Type::Wait:				o = [NSCursor ]; break;
	}
	
	if (!o) {
		AX_ASSERT(false);
		o = [NSCursor arrowCursor];
	}
	[o set];
	return true;
}

bool axNativeUIApp_MacOSX::onSetNativeCursorPos(const axVec2f& worldPos) {
	CGError err = ::CGWarpMouseCursorPosition(worldPos.to_CGPoint());
//	AX_DUMP_VAR(worldPos);	
	return err != kCGErrorSuccess;
}

#endif //#if AX_OS_MACOSX

