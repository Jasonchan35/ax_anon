#pragma once

#if AX_USE_VULKAN

#include "../../shader/axShader.h"
#include "axVkShaderPass.h"

class axVkShader : public axShader {
	AX_RTTI_CLASS(axVkShader, axShader)
public:
	using Util = axVkUtil;
	using DataType = axRenderDataType;

	axVkShaderPass* findPass(axInt i) { return static_cast<axVkShaderPass*>(Base::findPass(i)); }
};

#endif //AX_USE_VULKAN
