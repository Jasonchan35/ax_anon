#pragma once
#include "../Base/axNativeUIApp_Base.h"

class axNativeUIWindow_Null;
class axNativeUIApp_Null : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_Null, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using NativeWindow = axNativeUIWindow_Null;

	axNativeUIApp_Null() {}
	virtual ~axNativeUIApp_Null() {}

	virtual int		onRun		() override { return _returnCode; }
	virtual	void	quit		(int returnCode) override { _returnCode = returnCode; }

	virtual bool	onRequestNativeCustomAppEvent() override { return false; }

	static axVec2f	s_getVirtualDesktopSize() { return axVec2f(0); }
};