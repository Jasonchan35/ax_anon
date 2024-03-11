#pragma once

#if AX_USE_OPENGL

#include "../../shader/axMaterialPass.h"
#include "axGLShaderPass.h"

class axGLRenderContext_Base;

class axGLMaterialPass : public axMaterialPass {
	AX_RTTI_CLASS(axGLMaterialPass, axMaterialPass);
public:
	using Util = axGLUtil;

	void bind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);
	void unbind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);

	void bind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);
	void unbind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);

	void _bindParams(axGLCommandDispatcher& dispatcher);
	void _unbindParams(axGLCommandDispatcher& dispatcher);

	axGLShaderPass*	shaderPass() { return ax_type_cast_debug<axGLShaderPass>(Base::shaderPass()); }

private:

};

#endif
