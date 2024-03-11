#pragma once

#include "../Base/axNativeUIWindow_Base.h"

class axNativeUIWindow_Null : public axNativeUIWindow_Base {
	AX_RTTI_CLASS(axNativeUIWindow_Null, axNativeUIWindow_Base);
public:
	axNativeUIWindow_Null(CreateDesc& desc) : Base(desc) {}
	virtual ~axNativeUIWindow_Null() {}
	
	void onCreate(CreateDesc& desc) {}
};
