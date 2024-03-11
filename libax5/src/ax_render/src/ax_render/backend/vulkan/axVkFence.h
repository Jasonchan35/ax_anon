#pragma once

#if AX_USE_VULKAN

#include "axCppVk/axCppVk.h"
#include "../../axRenderFence.h"
#include "axVkCommonBase.h"

class axVkFence : public axRenderFence {
	AX_RTTI_CLASS(axVkFence, axRenderFence);
public:
	using Util = axVkUtil;

	axVkFence() {
		_fence.create(Util::rootDevice(), false);
	}

	bool onCheckCompleted() override {
		if (!_fence) throw axError_Undefined(AX_LOC);
		return _fence.check(false);
	}

	axCppVkFence _fence;
};

#endif