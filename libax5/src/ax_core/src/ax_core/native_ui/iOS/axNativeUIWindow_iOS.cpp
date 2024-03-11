//
//  axNativeUIWindow_iOS.cpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#if AX_OS_IOS

#include "axNativeUIWindow_iOS.h"

@interface axNativeUIWindow_iOS_UIViewController :  UIViewController {
}
@end

@implementation axNativeUIWindow_iOS_UIViewController
@end

#pragma mark ===================================================

@interface axNativeUIWindow_iOS_UIWin : UIWindow {
	axNativeUIWindow_iOS* _cpp;
}
@end

@implementation axNativeUIWindow_iOS_UIWin
- (instancetype) init:(axNativeUIWindow_iOS*)cpp {
	self = [super init];
	_cpp = cpp;
	return self;
}

-(void)setFrame:(CGRect)frame {
	[super setFrame:frame];
//	axGLApp::getInstance()->_os_onResize();
}
@end

#pragma mark ===================================================
@implementation axNativeUIWindow_iOS_UIView
- (instancetype) init:(axNativeUIWindow_iOS*)cpp {
	self = [super init];
	_cpp = cpp;
	return self;
}
@end


#pragma mark ===================================================

axNativeUIWindow_iOS::axNativeUIWindow_iOS(CreateDesc& desc)
	: Base(desc)
{
	_win = [[axNativeUIWindow_iOS_UIWin alloc] init:this];
	_rootView = [[axNativeUIWindow_iOS_UIView alloc] init:this];

	auto* _viewCtl = [[axNativeUIWindow_iOS_UIViewController alloc] init];

	[_win setRootViewController:_viewCtl];

	[_win addSubview:_rootView];
	[_win makeKeyAndVisible];
	
	CGRect screenRect = [[UIScreen mainScreen] bounds];
	[_win setFrame: screenRect];
	[_win setNeedsDisplay];
}

axNativeUIWindow_iOS::~axNativeUIWindow_iOS() {
}

void axNativeUIWindow_iOS::onSetNativeActive(bool b) {
}

void axNativeUIWindow_iOS::onSetNativeWindowTitle(axStrView title) {
}

void axNativeUIWindow_iOS::onSetNativeWorldPos(const axVec2f& v) {
	auto rc = [_win frame];
	rc.origin = v;

	// flip Y
	CGRect screenRect = [UIScreen mainScreen].bounds;
	rc.origin.y = screenRect.size.height - rc.origin.y - rc.size.height - 1;

	[_win setFrame:rc];
}

void axNativeUIWindow_iOS::onSetNativeSize(const axVec2f& v) {
	auto rc =[_win frame];
	rc.size = v;
	[_win setFrame:rc];

	rc.origin.x = 0;
	rc.origin.y = 0;
	[_rootView setFrame:rc];
}

void axNativeUIWindow_iOS::onSetNativeVisible(bool b) {
}

void axNativeUIWindow_iOS::doCloseWindow() {
}

void axNativeUIWindow_iOS::doDestroyWindow() {
}


#endif // AX_OS_IOS
