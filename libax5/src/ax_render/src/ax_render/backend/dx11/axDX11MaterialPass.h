#pragma once

#if AX_USE_DX11

#include "../../shader/axMaterialPass.h"
#include "axDX11ShaderPass.h"

class axDX11MaterialPass : public axMaterialPass {
	AX_RTTI_CLASS(axDX11MaterialPass, axMaterialPass);
public:
	using Util = axDX11Util;

	void bind	(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall);

	void bind	(axDX11CommandDispatcher& dispatcher, ComputeCall& compteCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, ComputeCall& compteCall);

	axDX11ShaderPass*	shaderPass() { return ax_type_cast_debug<axDX11ShaderPass>(Base::shaderPass()); }
private:
	void _bindParams	(axDX11CommandDispatcher& dispatcher);
	void _unbindParams	(axDX11CommandDispatcher& dispatcher);

};

#endif