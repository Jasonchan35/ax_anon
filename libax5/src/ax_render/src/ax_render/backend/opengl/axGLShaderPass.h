#pragma once

#if AX_USE_OPENGL

#include "../../shader/axShaderPass.h"
#include "axGLRenderer.h"

class axGLRenderContext_Base;
class axGLShaderPass : public axShaderPass {
	AX_RTTI_CLASS(axGLShaderPass, axShaderPass)
public:
	using Util = axGLUtil;

	axGLShaderPass();
	~axGLShaderPass();

	virtual void onInit	(axInt passIndex, const Info& info) override;
	virtual bool onValidate() final;

	void _compileStage(GLuint& stage, GLenum stageType, axStrView filename);
	bool _linkProgram();

	void bind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);
	void unbind	(axGLCommandDispatcher& dispatcher, DrawCall& drawCall);

	void bind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);
	void unbind	(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall);

	void _bindVertexInputLayout(DrawCall& drawCall);
	void _bindRenderState(bool debugWireframe);

	class BlendFuncConverter;

	void _initVertexAttrs();
	void _initUniformBuffers();
	void _initUniformVariables(GLuint blockIndex);
	void _initTexParams();
	void _initStorageBufferParams();

	StageFlags _getActiveUniformBlock_StageFlags(GLuint program, GLuint blockIndex);

	GLuint	_program = 0;
	GLuint	_vertexStage = 0;
	GLuint	_pixelStage = 0;
	GLuint	_computeStage = 0;
};

class axGLShaderPass::BlendFuncConverter {
public:
	using Util = axGLUtil;

	BlendFuncConverter(const axRenderState::BlendFunc& f) {
		if (f.op == BlendOp::Disable) {
			op = GL_FUNC_ADD;
			srcFactor = GL_ONE;
			dstFactor = GL_ZERO;
		}else{
			op			= Util::getGLBlendOp(f.op);
			srcFactor	= Util::getGLBlendFactor(f.srcFactor);
			dstFactor	= Util::getGLBlendFactor(f.dstFactor);
		}
	}

	GLenum  op;
	GLenum  srcFactor;
	GLenum	dstFactor;
};

#endif //AX_USE_OPENGL
