#pragma once

#if AX_OS_ANDROID
#include "../Base/axNativeUIWindow_Base.h"

class axNativeUIWindow_Android : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_Android, axNativeUIWindow_Base);
public:
	virtual ~axNativeUIWindow_Android();
	
	axNativeUIWindow_Android(CreateDesc& desc);
	
	void onCreate(CreateDesc& desc);
	virtual void onSetNativeActive(bool b) override;
	virtual void onSetNativeWindowTitle(axStrView title) override;
		
protected:
	virtual void onSetNativeWorldPos	(const axVec2f& v) override;
	virtual void onSetNativeSize		(const axVec2f& v) override;
	virtual void onSetNativeVisible	(bool b) override;
	virtual void doCloseWindow	() override;
	virtual void doDestroyWindow() override;
public:
};

#endif //#if AX_OS_ANDROID
