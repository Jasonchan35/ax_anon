//
//  axNativeUIApp_iOS.cpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#if AX_OS_IOS

#include "axNativeUIApp_iOS.h"

@interface axNativeUIApp_iOS_Delegate : NSObject < UIApplicationDelegate > {

}
@end

@interface axNativeUIApp_iOS_UIApp : UIApplication {
	axNativeUIApp_iOS* _cpp;
}
- (void)setCpp:(axNativeUIApp_iOS*)cpp;
@end

@implementation axNativeUIApp_iOS_Delegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	auto* cpp = axNativeUIApp_iOS::s_instance();
	auto* app = (axNativeUIApp_iOS_UIApp*)application;

	[app setCpp:cpp];
	cpp->_uiApp = app;
	cpp->create();
	return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
	axNativeUIApp_iOS* app = axNativeUIApp_iOS::s_instance();
	app->willQuit();
}
@end

@implementation axNativeUIApp_iOS_UIApp
- (void)setCpp:(axNativeUIApp_iOS*)cpp {
	_cpp = cpp;
}
- (void)sendEvent:(UIEvent *)ev {
	// Ref: Cocoa Event Handling Guide
	// NSApplicationDefined is of real use to application programs.
	// It allows the application to generate custom events and insert them into the event queue.
	// Each such event can have a subtype and two additional codes to differentiate it from others.
	// The NSEvent method otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:
	// creates one of these events, and the subtype, data1, and data2 methods return the information specific to these events.

//	using SubType = axNativeUIApp_iOS::CustomUIEventSubType;
//
//	if (NSInteger(ev.type) == axNativeUIApp::CustomUIEventType) {
//		switch (SubType(ev.subtype)) {
//			case SubType::LayoutRequest: {
//				axNativeUIApp::s_instance()->updateLayout();
//			}break;
//		}
//		return;
//	}
	[super sendEvent:ev];
	if (_cpp) {
		_cpp->updateLayout(false);
	}
}
- (void)onPostLayoutTimer:(NSTimer*)timer {
	if (_cpp) {
		_cpp->updateLayout(false);
	}
}
@end

axNativeUIApp_iOS::axNativeUIApp_iOS() {
	[axNativeUIApp_iOS_UIApp sharedApplication];
		
	auto resourcePath = [[NSBundle mainBundle] resourcePath];
	axPath::setCurrentDir(axStrView_make(resourcePath));
}

axNativeUIApp_iOS::~axNativeUIApp_iOS() {
}

bool axNativeUIApp_iOS::onPostLayoutRequest() {
	[NSTimer scheduledTimerWithTimeInterval:0 target:_uiApp selector:@selector(onPostLayoutTimer:) userInfo:nil repeats:NO];
	return true;
}

int	axNativeUIApp_iOS::onRun() {
	axString name("axNativeUIApp_iOS");
	char* argv[] = {name.data()};
	UIApplicationMain(0, argv, @"axNativeUIApp_iOS_UIApp", @"axNativeUIApp_iOS_Delegate");
	return _returnCode;
}

void axNativeUIApp_iOS::quit(int returnCode) {
	AX_ASSERT_NOT_IMPLEMENTED();
}

axVec2f	axNativeUIApp_iOS::s_getVirtualDesktopSize() {
	auto* s = [UIScreen mainScreen];
	auto rc = [s bounds];
	return axVec2f(rc.size);
}

#endif // AX_OS_IOS
