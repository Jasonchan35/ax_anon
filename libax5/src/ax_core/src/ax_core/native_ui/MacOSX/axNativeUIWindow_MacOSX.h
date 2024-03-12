#pragma once

#if AX_OS_MACOSX
#include "../Base/axNativeUIWindow_Base.h"
#include "axNativeUIEventUtil_MacOSX.h"
#include "../axNativeUIScreen.h"

class axNativeUIWindow_MacOSX;

@class axNativeUIWindow_MacOSX_NSWinDelegate;
@class axNativeUIWindow_MacOSX_ContentView;

@interface axNativeUIWindow_MacOSX_NSWin : NSWindow {
	axNativeUIWindow_MacOSX* _cpp;
}
- (void)onCustomDestroyWindow;
- (void)setCpp:(axNativeUIWindow_MacOSX*)cpp;
@end

class axNativeUIWindow_MacOSX : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_MacOSX, axNativeUIWindow_Base);
public:
	axNativeUIWindow_MacOSX(CreateDesc& desc);
	virtual ~axNativeUIWindow_MacOSX();
	
	void onCreate(CreateDesc& desc);
	virtual void onSetNativeActive(bool b) override;
	virtual void onSetNativeVisible(bool b) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
	virtual void doCloseWindow() override;
	virtual void doDestroyWindow() override;

	void _handleNativeUIMouseEvent	(NSEvent* inEvent);
	void _handleNativeUIKeyEvent	(NSEvent* inEvent);

	axNativeUIScreen	screen		();
	NSWindow*	nsWin		();
	NSView*		contentView	();

	axRect2f getWorldRect() const;
	
protected:
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;

public:
	using EventUtil = axNativeUIEventUtil_MacOSX;

	axNSObj<axNativeUIWindow_MacOSX_NSWin>			_nswin;
	axNSObj<axNativeUIWindow_MacOSX_NSWinDelegate>	_delegate;
	axNSObj<axNativeUIWindow_MacOSX_ContentView>	_contentView;

private:
	void _translateChar(NSEvent* ev);
};

#endif //#if AX_OS_MACOSX
