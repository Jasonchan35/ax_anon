#pragma once

#if AX_USE_Gtk
#include "Base/axNativeUIWindow_Base.h"

class axNativeUIWindow_Gtk : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_Gtk, axNativeUIWindow_Base);
public:
	axNativeUIWindow_Gtk(CreateDesc& desc);
	virtual ~axNativeUIWindow_Gtk();
	
	void onCreate(CreateDesc& desc);
	virtual void onSetNativeActive(bool b) override;
	virtual void onSetNativeVisible(bool b) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;

	GtkWindow*	nativeWin() { return _win; }
	GtkFixed*   nativeRootWidget() { return _rootWidget; }

private:
	GtkWindow* _win = nullptr;
	GtkFixed*  _rootWidget = nullptr;
};

#endif //#if AX_USE_Gtk
