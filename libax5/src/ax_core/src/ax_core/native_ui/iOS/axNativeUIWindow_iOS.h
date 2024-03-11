//
//  axNativeUIWindow_iOS.hpp
//  libax_core
//
//  Created by Jason on 2017-02-11.
//
//

#pragma once

#if AX_OS_IOS
#include "Base/axNativeUIWindow_Base.h"

class axNativeUIWindow_iOS;

@class axNativeUIWindow_iOS_UIWin;
@interface axNativeUIWindow_iOS_UIView : UIView {
	axNativeUIWindow_iOS* _cpp;
}
@end

class axNativeUIWindow_iOS : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_iOS, axNativeUIWindow_Base);
public:
	virtual ~axNativeUIWindow_iOS();
	
	axNativeUIWindow_iOS(CreateDesc& desc);
	
	void onCreate(CreateDesc& desc);
	virtual void onSetNativeActive(bool b) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
	
	axNativeUIWindow_iOS_UIWin*  win() { return _win.ptr(); }
	axNativeUIWindow_iOS_UIView* rootView() { return _rootView.ptr(); }

protected:
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;
	virtual void onSetNativeVisible	(bool b) override;
	virtual void doCloseWindow	() override;
	virtual void doDestroyWindow() override;
	
	axNSObj<axNativeUIWindow_iOS_UIWin> _win;
	axNSObj<axNativeUIWindow_iOS_UIView> _rootView;
};

#endif //#if AX_OS_IOS
