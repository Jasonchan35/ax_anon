#if AX_USE_METAL

#include "axMTLMaterial.h"
#include "axMTLTexture.h"
#include "axMTLRenderContext.h"

axMTLMaterial::~axMTLMaterial() {
	
}

void axMTLMaterial::onSetShader(Shader* newShader) {
}

void axMTLMaterial::bind(axMTLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axMTLMaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->bind(dispatcher, drawCall);
}

void axMTLMaterial::bind(axMTLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axMTLMaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->bind(dispatcher, computeCall);
}

#endif
