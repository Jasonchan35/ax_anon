#pragma once

#if AX_USE_OPENGL

#include "../../shader/axMaterial.h"
#include "axGLGpuBuffer.h"
#include "axGLShader.h"

class axGLRenderContext_Base;

class axGLMaterial : public axMaterial {
	AX_RTTI_CLASS(axGLMaterial, axMaterial);
public:
	using Util = axGLUtil;

	axGLMaterial() = default;
	~axGLMaterial();

	axGLShader*	shader() { return ax_type_cast_debug<axGLShader>(Base::shader()); }

	void bind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);
	void unbind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);

	void bind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);
	void unbind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);
};

#endif
