#pragma once

#include "axRenderDataType.h"
#include "axRenderCommonBase.h"

class axRenderResource : public axObject, public axRenderCommonBase {
	AX_RTTI_CLASS(axRenderResource, axObject)
public:
	axRenderResource();

	virtual void onRefCountZero() override {
		// TODO send resource to renderer to clean up in render thread
		ax_delete(this);
	}
};