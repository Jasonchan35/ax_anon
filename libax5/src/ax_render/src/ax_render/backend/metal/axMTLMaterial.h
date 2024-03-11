#pragma once

#include "../../shader/axMaterial.h"
#include "axMTLShader.h"

#if AX_USE_METAL

class axMTLMaterial : public axMaterial {
	AX_RTTI_CLASS(axMTLMaterial, axMaterial);
public:

	using Util = axMTLUtil;

	axMTLMaterial() = default;
	~axMTLMaterial();

	axMTLShader*	shader() { return static_cast<axMTLShader*>(Base::shader()); }

	virtual void onSetShader(Shader* newShader) override;

	void bind	(axMTLCommandDispatcher& dispatcher, DrawCall& 		drawCall);
	void bind	(axMTLCommandDispatcher& dispatcher, ComputeCall&	computeCall);
};

#endif // #if AX_USE_METAL
