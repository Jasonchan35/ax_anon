//
//  axNativeUIApp_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_IOS
#include "axNativeUIApp_Base.h"

@class axNativeUIApp_iOS_Delegate;
@class axNativeUIApp_iOS_UIApp;
class axNativeUIWindow_iOS;

class axNativeUIApp_iOS : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_iOS, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	static const NSInteger CustomUIEventType = 0xF0;
	enum class CustomUIEventSubType : long {
		Unknown,
		CustomDestroyWindow,
		LayoutRequest,
	};
	AX_STATIC_ASSERT(sizeof(CustomUIEventSubType) == sizeof(UIEventSubtype));

	using NativeWindow = axNativeUIWindow_iOS;
	
	axNativeUIApp_iOS();
	virtual ~axNativeUIApp_iOS();
	
	virtual bool	onPostLayoutRequest() override;
	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;

	static axVec2f	s_getVirtualDesktopSize();

	axNativeUIApp_iOS_UIApp* _uiApp = nil;
};

#endif //#if AX_OS_IOS
