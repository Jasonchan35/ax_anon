#include "axDX11Material.h"
#include "axDX11MaterialPass.h"
#include "axDX11StageBuffer.h"
#include "axDX11StorageBuffer.h"
#include "../../axRenderDrawCall.h"
#include "../../axRenderComputeCall.h"

#if AX_USE_DX11

void axDX11Material::bind(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axDX11MaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->bind(dispatcher, drawCall);
}

void axDX11Material::bind(axDX11CommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axDX11MaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->bind(dispatcher, computeCall);
}

void axDX11Material::unbind(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axDX11MaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->unbind(dispatcher, drawCall);
}

void axDX11Material::unbind(axDX11CommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axDX11MaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->unbind(dispatcher, computeCall);
}

#endif //#if AX_USE_DX11
