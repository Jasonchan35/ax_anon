#include "axMaterial.h"
#include "axShader.h"
#include "../texture/axTexture.h"
#include "../axRenderContext.h"

void axMaterial::setShader(Shader* shader) {
	if (!shader) return;

	_shader.ref(shader);
	_passes.resize(shader->passCount());

	axInt passIndex = 0;
	for (auto& pass : _passes) {
		pass = axRenderer::s_instance()->createMaterialPass();

		auto* shaderPass = shader->findPass(passIndex);
		if (!shaderPass) throw axError_Undefined(AX_LOC);

		pass->setShaderPass(shaderPass);
		passIndex++;
	}

	onSetShader(shader);
}

void axMaterial::onUpdateGpuResource(axRenderRequest& req) {
	for (auto& pass : _passes) {
		pass->onUpdateGpuResource(req);
	}
}
