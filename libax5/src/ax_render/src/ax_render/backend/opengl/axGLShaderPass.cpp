#if AX_USE_OPENGL

#include "axGLShader.h"
#include "axGLShaderPass.h"
#include "axGLRenderContext.h"
#include "axGLRenderer.h"

axGLShaderPass::axGLShaderPass() {
}

axGLShaderPass::~axGLShaderPass() {
	if (_vertexStage)	{
		if (_program) glDetachShader(_program, _vertexStage);
		glDeleteShader(_vertexStage);
		_vertexStage = 0;
	}
	if (_pixelStage) {
		if (_program) glDetachShader(_program, _pixelStage);
		glDeleteShader(_pixelStage);
		_pixelStage = 0;
	}
	if (_computeStage) {
		if (_program) glDetachShader(_program, _computeStage);
		glDeleteShader(_computeStage);
		_computeStage = 0;
	}
	if (_program) { 
		glDeleteProgram(_program);
		_program = 0; 
	}
}

void axGLShader_getProgramInfoLog(GLuint program, axIString8 &out_msg) {
	out_msg.clear();
	if (!program) return;
	
	GLsizei	bufLen = 0;
	GLsizei outLen = 0;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLen);
	out_msg.resize(bufLen + 1);
	
	glGetProgramInfoLog(program, bufLen, &outLen, out_msg.data());
	out_msg.resize(outLen);
}

void axGLShader_getShaderInfoLog(GLuint shader, axIString8 &out_msg) {
	out_msg.clear();
	if (!shader) return;
	
	GLsizei	bufLen = 0;
	GLsizei outLen = 0;
	
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufLen);
	out_msg.resize(bufLen + 1);
	
	glGetShaderInfoLog(shader, bufLen, &outLen, out_msg.data());
	out_msg.resize(outLen);
}

void axGLShader_logProgramInfo(GLuint program, axStrView title, axStrView filename) {
	axTempString errorMsg;
	axGLShader_getProgramInfoLog(program, errorMsg);
	AX_LOG("{?}\n  filename = {?}\n{?}\n", title, filename, errorMsg );
}

void axGLShader_logShaderInfo(GLuint shader, axStrView title, axStrView filename) {
	axTempString errorMsg;
	axGLShader_getShaderInfoLog(shader, errorMsg);
	AX_LOG("{?}\n   filename = {?}\n{?}\n", title, filename, errorMsg);
}

void axGLShaderPass::_compileStage(GLuint& stage, GLenum stageType, axStrView filename) {
	if (!stage) {
		stage = glCreateShader(stageType);
	}

	axMemMap mm(filename);
	auto sourceCode = mm.strView8();

	GLint srcLen = (GLint)sourceCode.size();
	const GLchar* srcData = sourceCode.data();

	glShaderSource(stage, 1, &srcData, &srcLen);
	glCompileShader(stage);

	GLint compiled;
	glGetShaderiv(stage, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		axGLShader_logShaderInfo(stage, "Error: compile shader", filename);
		throw axError_Undefined(AX_LOC);
	}  
}

void axGLShaderPass::onInit(axInt passIndex, const Info& info) {
	auto filename = shader()->filename();

#if AX_USE_OpenGL_ES
	axStrLiteral passName = "GLSL_ES_pass";
	axStrLiteral vsProfile = StageProfile::GLSL_ES_VS;
	axStrLiteral psProfile = StageProfile::GLSL_ES_PS;
	axStrLiteral csProfile = StageProfile::GLSL_ES_CS;
#else
	axStrLiteral passName = "GLSL_pass";
	axStrLiteral vsProfile = StageProfile::GLSL_VS;
	axStrLiteral psProfile = StageProfile::GLSL_PS;
	axStrLiteral csProfile = StageProfile::GLSL_CS;
#endif

	if (info.vsFunc) {
		axTempString tmp;
		tmp.format("{?}/{?}{?}.{?}.glsl", filename, passName, passIndex, vsProfile);
		_compileStage(_vertexStage, GL_VERTEX_SHADER, tmp);
	}
	if (info.psFunc) {
		axTempString tmp;
		tmp.format("{?}/{?}{?}.{?}.glsl", filename, passName, passIndex, psProfile);
		_compileStage(_pixelStage, GL_FRAGMENT_SHADER, tmp);
	}
	if (info.csFunc) {
		axTempString tmp;
		tmp.format("{?}/{?}{?}.{?}.glsl", filename, passName, passIndex, csProfile);
		_compileStage(_computeStage, GL_COMPUTE_SHADER, tmp);
	}

	_linkProgram();
}

bool axGLShaderPass::_linkProgram() {
	if (!_program) {
		_program = glCreateProgram();
	}

	axTempString filename;
	filename.format("{?}-pass{?}", shader()->filename(), passIndex());
	
	if (_vertexStage) {
		glAttachShader(_program, _vertexStage);
		Util::checkGLError(AX_LOC);
	}

	if (_pixelStage) {
		glAttachShader(_program, _pixelStage);
		Util::checkGLError(AX_LOC);
	}

	if (_computeStage) {
		glAttachShader(_program, _computeStage);
		Util::checkGLError(AX_LOC);
	}

	{
		glLinkProgram(_program);
		GLint linked;
		glGetProgramiv(_program, GL_LINK_STATUS, &linked);
		if (!linked) {
			axGLShader_logProgramInfo(_program, "Error: link shader", filename);
			throw axError_Undefined(AX_LOC);
		}
	}
	
	_initVertexAttrs();
	_initUniformBuffers();
	_initTexParams();
	_initStorageBufferParams();
	return true;
}

void axGLShaderPass::_initVertexAttrs() {
	GLint activeCount = 0;
	GLint nameMaxLen  = 0;
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES,           &activeCount);
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameMaxLen);

	_vertexAttrs.reserve(activeCount);
	axTempString8 attrName;

	auto filename = shader()->filename();

	for (GLint i = 0; i < activeCount; i++) {
		GLsizei outLen = 0;
		GLint   dataSize = 0;
		GLenum  dataType = 0;
		attrName.resize(nameMaxLen);
		glGetActiveAttrib(_program, (GLuint)i, nameMaxLen, &outLen, &dataSize, &dataType, attrName.data());
		attrName.resize(outLen);

		auto loc = glGetAttribLocation(_program, attrName.c_str());
		if (loc < 0) {
			AX_LOG("OpenGLShader invalid vertex attr {?}\nfile:{?}", attrName, filename);
		}

		auto attrId = axShaderVertexAttrIdHelper::fromVarName(attrName);
		if (attrId == VertexAttrId::Unknown) {
			AX_LOG("OpenGLShader Unknown vertex attr {?}\nfile:{?}", attrName, filename);
			throw axError_Undefined(AX_LOC);
		}

		_addVertexAttr(attrId, Util::getAxDataType(dataType), loc);
	}
}

void axGLShaderPass::_initUniformBuffers() {
	GLint activeBlockCount;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &activeBlockCount);

	_uniformBufferMap.reserve(activeBlockCount);

	axTempString8 blockName;

	for (GLint i = 0; i < activeBlockCount; i++) {
		GLuint idx = static_cast<GLuint>(i);
		GLint nameMaxLen = 0;
		glGetActiveUniformBlockiv(_program, idx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameMaxLen);

		GLsizei outLen;
		blockName.resize(nameMaxLen);
		glGetActiveUniformBlockName(_program, idx, nameMaxLen, &outLen, blockName.data());
		blockName.resize(outLen);

		auto blockIndex = Util::castGLuint(i);

		GLint dataSize = 0;
		glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);
		Util::checkGLError(AX_LOC);

		GLint bindPoint;
		glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &bindPoint);
		Util::checkGLError(AX_LOC);

		auto stageFlags = _getActiveUniformBlock_StageFlags(_program, blockIndex);

		auto loc = glGetUniformBlockIndex(_program, blockName.c_str());
		Util::checkGLError(AX_LOC);
		AX_ASSERT(loc != GL_INVALID_INDEX);

		if (dataSize <= 0) throw axError_Undefined(AX_LOC);
		_addUniformBuffer(stageFlags, axNameId::s_make(blockName), dataSize, bindPoint, loc);

		_initUniformVariables(blockIndex);
	}
}

void axGLShaderPass::_initUniformVariables(GLuint blockIndex) {
	GLint varCount = -1;
	glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &varCount);
	Util::checkGLError(AX_LOC);

	axArray<GLint, 128> varIndices;
	varIndices.resize(varCount);
	glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, varIndices.data());
	Util::checkGLError(AX_LOC);

	auto stageFlags = _getActiveUniformBlock_StageFlags(_program, blockIndex);

	axArray<GLint, 128> varOffsets;
	varOffsets.resize(varCount);
	glGetActiveUniformsiv(_program, varCount, (GLuint*)varIndices.data(), GL_UNIFORM_OFFSET, varOffsets.data());
	Util::checkGLError(AX_LOC);

	GLint activeUniformCount = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &activeUniformCount);
	Util::checkGLError(AX_LOC);

	_paramMap.reserve(activeUniformCount);

	{// uniform variables
		axTempString paramFullName;
		for (GLint i = 0; i < varCount; i++) {
			GLsizei outLen;
			GLint nameMaxLen;
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameMaxLen);
			Util::checkGLError(AX_LOC);

			GLint   gl_dataSize;
			GLenum  gl_dataType;

			paramFullName.resize(nameMaxLen);
			glGetActiveUniform(_program, (GLuint)varIndices[i], nameMaxLen, &outLen, &gl_dataSize, &gl_dataType, paramFullName.data());
			Util::checkGLError(AX_LOC);

			paramFullName.resize(outLen);

			AX_ASSERT(gl_dataSize == 1);

			auto dataType = Util::getAxDataType(gl_dataType);
			auto paramName = axNameId::s_make(paramFullName.splitByChar('.').second);

			_addUniformParam(stageFlags, paramName, dataType, blockIndex, varOffsets[i]);
		}
	}
}

void axGLShaderPass::_initTexParams() {
	axTempString8 texName;

	GLint activeUniformCount = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &activeUniformCount);

	for (GLint i = 0; i < activeUniformCount; i++) {
		GLint nameMaxLen = 0;
		glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameMaxLen);

		GLint   gl_dataSize;
		GLenum  gl_dataType;
		GLsizei outLen;

		texName.resize(nameMaxLen);
		glGetActiveUniform(_program, static_cast<GLuint>(i), nameMaxLen, &outLen, &gl_dataSize, &gl_dataType, texName.data());
		texName.resize(outLen);

		AX_ASSERT(gl_dataSize == 1);

		auto dataType = Util::getAxDataType(gl_dataType);
		if (!DataTypeUtil::isTexture(dataType))
			continue;

		auto stageFlags = StageFlags::Unknown;
	#if 0 // require GL4.3 - glGetProgramResourceiv
		{
			GLenum props[] = {
				GL_REFERENCED_BY_VERTEX_SHADER,
				GL_REFERENCED_BY_FRAGMENT_SHADER
			};
			const auto n = ax_c_array_size(props);
			GLint values[n] = {};

			glGetProgramResourceiv(_program, GL_UNIFORM, static_cast<GLuint>(i), n, props, n, nullptr, values);

			if (values[0]) stageFlags.setBits(StageFlags::Vertex);
			if (values[1]) stageFlags.setBits(StageFlags::Pixel);
		}
	#endif
		auto paramName = axNameId::s_make(texName.split("_axSamplerState_").second);
		auto loc = glGetUniformLocation(_program, texName.c_str());
		_addTextureParam(stageFlags, paramName, dataType, loc);
	}
}

void axGLShaderPass::_initStorageBufferParams() {
#if 1 // require GL4.3 - glGetProgramResourceiv
	if (!_computeStage) return;

	axTempString8 blockName;
	const axInt nameMaxLen = 256;

	GLuint storageBufCount = 0;
	{
		GLint value = 0;
		glGetProgramInterfaceiv(_program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &value);
		Util::checkGLError(AX_LOC);

		storageBufCount = Util::castGLuint(value);
	}

	for (GLuint i = 0; i < storageBufCount; i++) {

		GLsizei outLen;
		blockName.resize(nameMaxLen);
		glGetProgramResourceName(_program, GL_SHADER_STORAGE_BLOCK, i, Util::castGLsizei(blockName.size()), &outLen, blockName.data());
		blockName.resize(outLen);

		axInt bindPoint = 0;
		{
			GLenum props = GL_BUFFER_BINDING;
			GLint value = 0;
			glGetProgramResourceiv(_program, GL_SHADER_STORAGE_BLOCK, i, 1, &props, 1, nullptr, &value);
			bindPoint = value;
		}

		auto stageFlags = StageFlags::Unknown;
		auto paramName = axNameId::s_make(blockName);
		_addStorageBufferParam(stageFlags, paramName, bindPoint, false);
	}
	Util::checkGLError(AX_LOC);

#endif
}

axGLShaderPass::StageFlags axGLShaderPass::_getActiveUniformBlock_StageFlags(GLuint program, GLuint blockIndex) {
	auto o = StageFlags::Unknown;

/* Doesn't support on Mac
	GLint tmp;

	tmp = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,   &tmp);
	if (tmp) o.setFlags(StageFlags::Vertex);
			
	tmp = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &tmp);
	if (tmp) o.setFlags(StageFlags::Pixel);

	tmp = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER, &tmp);
	if (tmp) o.setFlags(StageFlags::Compute);

	Util::checkGLError(AX_LOC);
	*/
	return o;
}

bool axGLShaderPass::onValidate() {
	if (!_program) return false;

	glValidateProgram(_program);
	GLint validated;
	glGetProgramiv(_program, GL_VALIDATE_STATUS, &validated);
	if (!validated) {
		axGLShader_logProgramInfo(_program, "Error: validate shader", shader()->filename());
		//throw axError_Undefined(AX_LOC);
		return false;
	}
	Util::checkGLError(AX_LOC);
	
	return true;
}

void axGLShaderPass::_bindRenderState(bool debugWireframe) {
	switch (_staticRenderState.cull) {
		case RenderState::Cull::Back:	glEnable(GL_CULL_FACE); glCullFace(GL_BACK);  break;
		case RenderState::Cull::Front:	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
		case RenderState::Cull::None:	glDisable(GL_CULL_FACE); break;
		default: throw axError_Undefined(AX_LOC);
	}

#if !AX_USE_OpenGLES
	glPointSize(_staticRenderState.pointSize);
#endif

	#if !AX_USE_OpenGLES
	if (_staticRenderState.antialiasedLine) {
		glEnable(GL_LINE_SMOOTH);
	}else{
		glDisable(GL_LINE_SMOOTH);
	}
	#endif

	glDepthFunc(Util::getGLDepthTestOp(_staticRenderState.depthTest.op));

#if !AX_USE_OpenGLES
	glPolygonMode(GL_FRONT_AND_BACK, _staticRenderState.wireframe || debugWireframe ? GL_LINE : GL_FILL);
#endif

	glDepthMask(_staticRenderState.depthTest.writeMask);

	if (_staticRenderState.depthTest.isEnable()) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	auto& blend = _staticRenderState.blend;
	if (blend.isEnable()) {
		#if AX_USE_OpenGLES
			glEnable(GL_BLEND);
		#else
			glEnablei(GL_BLEND, 0);
		#endif

		auto rgbOp			= Util::getGLBlendOp(blend.rgb.op);
		auto alphaOp		= Util::getGLBlendOp(blend.alpha.op);
		auto rgbSrcFactor	= Util::getGLBlendFactor(blend.rgb.srcFactor);
		auto rgbDstFactor	= Util::getGLBlendFactor(blend.rgb.dstFactor);
		auto alphaSrcFactor	= Util::getGLBlendFactor(blend.alpha.srcFactor);
		auto alphaDstFactor	= Util::getGLBlendFactor(blend.alpha.dstFactor);

		//require GL4.0 multiple render target
		//glBlendEquationSeparatei(0, rgbOp, alphaOp);
		glBlendEquationSeparate(rgbOp, alphaOp);
		Util::checkGLError(AX_LOC);

		//require GL4.0 multiple render target
		//glBlendFuncSeparatei(0, rgbSrcFactor, rgbDstFactor, alphaSrcFactor, alphaDstFactor);
		glBlendFuncSeparate(rgbSrcFactor, rgbDstFactor, alphaSrcFactor, alphaDstFactor);
		Util::checkGLError(AX_LOC);
	
	}else{
		#if AX_USE_OpenGLES
			glDisable(GL_BLEND);
		#else
			glDisablei(GL_BLEND, 0);
		#endif
	}
	const auto color = _staticRenderState.blend.constColor;
	glBlendColor(color.r, color.g, color.b, color.a);

}

void axGLShaderPass::_bindVertexInputLayout(DrawCall& drawCall) {
	checkAcceptVertexDesc(drawCall.vertexDesc);
	Util::checkGLError(AX_LOC);

	auto inputVertexDesc = drawCall.vertexDesc;
	auto strideBytes = inputVertexDesc->strideBytes;
	if (strideBytes <= 0) throw axError_Undefined(AX_LOC);
	
	for (auto& reqAttr : _vertexAttrs) {
		auto attrId = reqAttr.attrId;
		auto& attr = inputVertexDesc->attr(attrId);

		axShaderPass::validateVertexAttr(reqAttr, attr);

		auto offset = (void*)(attr.offset + drawCall.vertexByteOffset);

		GLenum dataType = Util::getGLDataType(attr.dataType);
		GLint  numElements = DataTypeUtil::elementCount(attr.dataType);

		auto attrLoc = Util::castGLuint(reqAttr.bindPoint);
		glEnableVertexAttribArray(attrLoc);
		Util::checkGLError(AX_LOC);

		const bool normalized = true;
		glVertexAttribPointer(attrLoc, numElements, dataType, normalized, strideBytes, offset);
		Util::checkGLError(AX_LOC);
	}
}

void axGLShaderPass::bind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	_bindVertexInputLayout(drawCall);
	_bindRenderState(drawCall.debugWireframe);

	glUseProgram(_program);
	Util::checkGLError(AX_LOC);
}

void axGLShaderPass::unbind(axGLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	glUseProgram(0);

	// unbind vertex input
	for (auto& shaderAttr : _vertexAttrs) {
		auto attrLoc = Util::castGLuint(shaderAttr.bindPoint);
		glDisableVertexAttribArray(attrLoc);
		//checkGLError(AX_LOC);
	}
}

void axGLShaderPass::bind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	glUseProgram(_program);
	Util::checkGLError(AX_LOC);
}

void axGLShaderPass::unbind(axGLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	glUseProgram(0);
}

#endif //AX_USE_OPENGL
