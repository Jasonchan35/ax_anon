#if AX_USE_VULKAN

#include <ax_render/axRenderContext.h>
#include <ax_render/axRenderDrawCall.h>

#include "axVkShaderPass.h"
#include "axVkMaterial.h"
#include "axVkMaterialPass.h"
#include "axVkTexture.h"

void axVkMaterial::bind(axVkCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axVkMaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->bind(dispatcher, drawCall);
}

void axVkMaterial::bind(axVkCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axVkMaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->bind(dispatcher, computeCall);
}

#endif
