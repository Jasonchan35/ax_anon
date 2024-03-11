#pragma once

#if AX_OS_ANDROID
#include "../Base/axNativeUIApp_Base.h"

class axNativeUIWindow_Android;

class axNativeUIApp_Android : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_Android, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using NativeWindow = axNativeUIWindow_Android;
	
	virtual ~axNativeUIApp_Android();
			
	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;
	
	static axVec2f	s_getVirtualDesktopSize();	
};

#endif //#if AX_OS_ANDROID
