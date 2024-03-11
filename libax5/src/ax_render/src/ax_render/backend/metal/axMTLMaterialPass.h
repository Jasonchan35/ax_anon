#pragma once

#if AX_USE_METAL

#include "../../shader/axMaterialPass.h"
#include "axMTLShaderPass.h"

class axMTLMaterialPass : public axMaterialPass {
	AX_RTTI_CLASS(axMTLMaterialPass, axMaterialPass);
public:
	using Util = axMTLUtil;

	void bind(axMTLCommandDispatcher& dispatcher, DrawCall& 	drawCall);
	void bind(axMTLCommandDispatcher& dispatcher, ComputeCall& 	computeCall);

	axMTLShaderPass*	shaderPass() { return ax_type_cast_debug<axMTLShaderPass>(Base::shaderPass()); }

	void _bindParams(id<MTLRenderCommandEncoder>  cmdEncoder);
	void _bindParams(id<MTLComputeCommandEncoder> cmdEncoder);
};

#endif
