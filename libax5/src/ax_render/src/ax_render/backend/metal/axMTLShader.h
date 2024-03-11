#pragma once

#if AX_USE_METAL

#include "../../shader/axShader.h"
#include "axMTLShaderPass.h"

class axMTLShader : public axShader {
	AX_RTTI_CLASS(axMTLShader, axShader)
public:
	using Util = axMTLUtil;
	using DataType = axRenderDataType;

	axMTLShaderPass* findPass(axInt i) { return static_cast<axMTLShaderPass*>(Base::findPass(i)); }
};

#endif //AX_USE_METAL
