#pragma once

#if AX_USE_OPENGL

#include "../../shader/axShader.h"
#include "axGLShaderPass.h"

class axGLShader : public axShader {
	AX_RTTI_CLASS(axGLShader, axShader)
public:
	using DataType = axRenderDataType;
	using Util = axGLUtil;

	axGLShaderPass* findPass(axInt i) { return static_cast<axGLShaderPass*>(Base::findPass(i)); }
};

#endif //AX_USE_OPENGL
