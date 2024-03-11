#include "axGLMaterialPass.h"
#include "axGLGpuBuffer.h"
#include "axGLRenderContext_Base.h"
#include "axGLCommandDispatcher.h"
#include "axGLTexture.h"
#include "axGLStorageBuffer.h"

#if AX_USE_OPENGL

void axGLMaterialPass::bind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(dispatcher, drawCall);
	_bindParams(dispatcher);
}

void axGLMaterialPass::bind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(dispatcher, computeCall);
	_bindParams(dispatcher);
}

void axGLMaterialPass::_bindParams(axGLCommandDispatcher& dispatcher) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	{ //const buffer
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			GLuint bindPoint = Util::castGLuint(cb.bindPoint());
			GLuint loc       = Util::castGLuint(cb.glLoc());

			auto* gpuBuf = ax_type_cast_debug<axGLGpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			glUniformBlockBinding(shaderPass_->_program, loc, bindPoint);
			glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, gpuBuf->handle(), 0, gpuBuf->bufferSize());
			Util::checkGLError(AX_LOC);

			cbIndex++;
		}
	}

	{ //bind texture
		GLint texUnit = 0;
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			GLint bindPoint = Util::castGLint(texParam.bindPoint());

			glUniform1i(bindPoint, texUnit);
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + texUnit));

			SamplerState samplerState;
			GLenum target;
			axInt mipmapCount = 1;

			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axGLTexture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);
					tex2d->bind();
					target = tex2d->kGLTarget;
					samplerState = tex2d->samplerState();
					mipmapCount = tex2d->mipmapCount();

					glTexParameteri(target, GL_TEXTURE_WRAP_S, Util::getGLTextureWrap(samplerState.wrapU));
					glTexParameteri(target, GL_TEXTURE_WRAP_T, Util::getGLTextureWrap(samplerState.wrapV));
				}break;

				default: throw axError_Undefined(AX_LOC);
			}

			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, Util::getGLTextureMinFilter(samplerState.filter, mipmapCount));
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, Util::getGLTextureMagFilter(samplerState.filter));

			texUnit++;
		}
		glActiveTexture(GL_TEXTURE0);
	}

	{
		for (auto& bufParam : _storageBufParams) {
			auto* sbuf = ax_type_cast_debug<axGLStorageBuffer>(bufParam.storageBuffer());
			if (!sbuf) continue;

			auto* gpuBuf = sbuf->gpuBuffer();
			GLuint bindPoint = Util::castGLuint(bufParam.bindPoint());

			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindPoint, gpuBuf->handle(), 0, gpuBuf->bufferSize());
		}
	}
}

void axGLMaterialPass::unbind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	_unbindParams(dispatcher);
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->unbind(dispatcher, drawCall);
}

void axGLMaterialPass::unbind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	_unbindParams(dispatcher);
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->unbind(dispatcher, computeCall);
}

void axGLMaterialPass::_unbindParams(axGLCommandDispatcher& dispatcher) {
	for (auto& cb : _uniformBuffers) {
		if (auto* gpuBuf = ax_type_cast_debug<axGLGpuBuffer>(cb.currentGpuBuffer())) {
			gpuBuf->unbind();
		}
	}

}

#endif
