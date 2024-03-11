#if AX_USE_OPENGL

#include "axGLMaterial.h"
#include "axGLMaterialPass.h"
#include "axGLTexture.h"
#include "axGLCommonBase.h"
#include "../../axRenderDrawCall.h"

axGLMaterial::~axGLMaterial() {
	
}

void axGLMaterial::bind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axGLMaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->bind(dispatcher, drawCall);
}

void axGLMaterial::unbind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* pass = ax_type_cast_debug<axGLMaterialPass>(_passes[drawCall.renderPass].ptr());
	pass->unbind(dispatcher, drawCall);
}

void axGLMaterial::bind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axGLMaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->bind(dispatcher, computeCall);
}

void axGLMaterial::unbind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* pass = ax_type_cast_debug<axGLMaterialPass>(_passes[computeCall.renderPass].ptr());
	pass->unbind(dispatcher, computeCall);
}

#endif
