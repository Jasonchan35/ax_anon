#pragma once

#include "../../shader/axMaterial.h"
#include "axDX12Shader.h"
#include "axDX12MaterialPass.h"

#if AX_USE_DX12

class axDX12Material : public axMaterial {
	AX_RTTI_CLASS(axDX12Material, axMaterial);
public:
	using Util = axDX12Util;

	axDX12Material() = default;

	axDX12Shader*	shader() { return ax_type_cast_debug<axDX12Shader>(Base::shader()); }

	void bind	(ID3D12GraphicsCommandList* cmdList, DrawCall&		drawCall);
	void bind	(ID3D12GraphicsCommandList* cmdList, ComputeCall&	computeCall);
};

#endif //#if AX_USE_DX12
