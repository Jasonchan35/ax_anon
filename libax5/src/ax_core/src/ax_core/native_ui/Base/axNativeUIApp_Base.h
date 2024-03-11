#pragma once

#include "../../app/axApp.h"
#include "axNativeUIWindow_Base.h"

#define axUINativeApi_EnumList(E) \
	E(Unknown,) \
	E(Win32,) \
	E(WinRT,) \
	E(X11,) \
	E(MacOSX_AppKit,) \
	E(iOS_UIKit,) \
//-----
ax_ENUM_CLASS(axUINativeApi, axInt8);

class axUIFontManager;
class axNativeUIWindow;

class axNativeUIApp_Base : public axApp {
	AX_RTTI_CLASS(axNativeUIApp_Base, axApp)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using Time = axNativeUIEventTime;

	axNativeUIApp_Base();

	virtual void	quit		(int returnCode) = 0;

	bool setCursor(axUICursorType type) { return onSetNativeCursor(type); }
	bool setCursorPos(const axVec2f& worldPos) { return onSetNativeCursorPos(worldPos); }

	void setNeedToRender() { onSetNeedToRender(); }

	void requestCustomAppEvent();
	void handleCustomAppEvent(bool force);

	void updateLayout() { onUpdateLayout(); }

protected:
	virtual bool onRequestNativeCustomAppEvent() = 0;
	virtual void onHandleCustomAppEvent() = 0;
	virtual void onSetNeedToRender() {}
	virtual void onUpdateLayout() {}
	virtual bool onSetNativeCursor(axUICursorType type) { return false; }
	virtual bool onSetNativeCursorPos(const axVec2f& worldPos) { return false; }

	bool _pendingCustomAppEvent : 1;
	int	_returnCode {0};
};
