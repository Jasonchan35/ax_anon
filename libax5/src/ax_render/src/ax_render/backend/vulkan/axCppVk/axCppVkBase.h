#pragma once

#if AX_USE_VULKAN

#include "../axVkCommonBase.h"

class axCppVkBase : public axNonCopyable {
public:
	using Util = axVkUtil;
};

#endif