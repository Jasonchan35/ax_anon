#include "../../ax_render_config.h"

#if AX_USE_DX12

#include "axDX12Texture.h"
#include "axDX12Material.h"
#include "axDX12RenderContext.h"

void axDX12Material::bind(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axDX12MaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->bind(cmdList, drawCall);
}

void axDX12Material::bind(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axDX12MaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->bind(cmdList, computeCall);
}

#endif //AX_USE_DX12
