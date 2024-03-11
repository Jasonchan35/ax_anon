#pragma once

#if AX_USE_VULKAN

#include "../../shader/axMaterial.h"
#include "axVkShader.h"

class axVkMaterial : public axMaterial {
	AX_RTTI_CLASS(axVkMaterial, axMaterial);
public:
	using Util = axVkUtil;

	axVkMaterial() = default;

	axVkShader*	shader() { return ax_type_cast_debug<axVkShader>(Base::shader()); }

	void bind	(axVkCommandDispatcher& dispatcher, DrawCall& drawCall);
	void bind	(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall);
};

#endif // #if AX_USE_VULKAN