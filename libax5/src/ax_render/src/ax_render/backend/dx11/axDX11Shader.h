#pragma once

#include "../../ax_render_config.h"

#if AX_USE_DX11

#include "axDX11ShaderPass.h"

class axDX11Shader : public axShader {
	AX_RTTI_CLASS(axDX11Shader, axShader)
public:
	using DataType = axRenderDataType;

	axDX11ShaderPass* findPass(axInt i) { return static_cast<axDX11ShaderPass*>(Base::findPass(i)); }
};

#endif //AX_USE_DX11
