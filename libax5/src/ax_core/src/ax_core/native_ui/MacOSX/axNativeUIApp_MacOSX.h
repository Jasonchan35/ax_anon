#pragma once

#if AX_OS_MACOSX
#include "../Base/axNativeUIApp_Base.h"

@class axNativeUIApp_MacOSX_NSDelegate;
class axNativeUIWindow_MacOSX;

class axNativeUIApp_MacOSX : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_MacOSX, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()
	
	using NativeWindow = axNativeUIWindow_MacOSX;

	axNativeUIApp_MacOSX();
	virtual ~axNativeUIApp_MacOSX();

	virtual bool	onRequestNativeCustomAppEvent() override;
	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;
	virtual bool 	onSetNativeCursor(axUICursorType type) override;
	virtual bool 	onSetNativeCursorPos(const axVec2f& worldPos) override;

	static axRect2f	s_getVirtualDesktopRect();

	axNSObj<axNativeUIApp_MacOSX_NSDelegate> _delegate;
};

#endif //#if AX_OS_MACOSX
