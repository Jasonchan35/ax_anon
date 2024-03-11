#pragma once

#include "../../ax_render_config.h"

#if AX_USE_DX12

#include "axDX12ShaderPass.h"

class axDX12Shader : public axShader {
	AX_RTTI_CLASS(axDX12Shader, axShader)
public:
	using Util = axDX12Util;
	using DataType = axRenderDataType;

	axDX12ShaderPass* findPass(axInt i) { return static_cast<axDX12ShaderPass*>(Base::findPass(i)); }
};

#endif //AX_USE_DX12
