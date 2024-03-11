#if AX_USE_OPENGL

#include "axGLCommonBase.h"
#include "axGLShader.h"

axGLRenderer* axGLUtil::renderer() {
	return axGLRenderer::s_instance();
}

axRenderDataType axGLUtil::getAxDataType(GLenum type) {
	using DST = axRenderDataType;
	switch (type) {
		case GL_SHORT:				return DST::Int16;
	//---
		case GL_INT:				return DST::Int32;
		case GL_INT_VEC2:			return DST::Int32x2;
		case GL_INT_VEC3:			return DST::Int32x3;
		case GL_INT_VEC4:			return DST::Int32x4;
	//---
		case GL_UNSIGNED_INT:		return DST::UInt32;
		case GL_UNSIGNED_INT_VEC2:	return DST::UInt32x2;
		case GL_UNSIGNED_INT_VEC3:	return DST::UInt32x3;
		case GL_UNSIGNED_INT_VEC4:	return DST::UInt32x4;
	//---
		case GL_FLOAT:				return DST::Float32;
		case GL_FLOAT_VEC2:			return DST::Float32x2;
		case GL_FLOAT_VEC3:			return DST::Float32x3;
		case GL_FLOAT_VEC4:			return DST::Float32x4;
		case GL_FLOAT_MAT2:			return DST::Float32_2x2;
		case GL_FLOAT_MAT3:			return DST::Float32_3x3;
		case GL_FLOAT_MAT4:			return DST::Float32_4x4;
		case GL_FLOAT_MAT4x3:		return DST::Float32_4x3;
	//---
		case GL_DOUBLE:				return DST::Float64;
		case GL_DOUBLE_VEC2:		return DST::Float64x2;
		case GL_DOUBLE_VEC3:		return DST::Float64x3;
		case GL_DOUBLE_VEC4:		return DST::Float64x4;
		case GL_DOUBLE_MAT2:		return DST::Float64_2x2;
		case GL_DOUBLE_MAT3:		return DST::Float64_3x3;
		case GL_DOUBLE_MAT4:		return DST::Float64_4x4;
		case GL_DOUBLE_MAT4x3:		return DST::Float64_4x3;
		
	#if !AX_USE_OpenGLES
		case GL_SAMPLER_1D:			return DST::Texture1D;
	#endif
		
		case GL_SAMPLER_2D:			return DST::Texture2D;
		case GL_SAMPLER_3D:			return DST::Texture3D;
		case GL_SAMPLER_CUBE:		return DST::TextureCube;
		default: throw axError_Undefined(AX_LOC);
	}
}

bool axGLUtil::checkGLError(const axSourceLoc& loc) {
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
#if _DEBUG
		axStrView errStr("Unknown");

		switch (err) {
		#define ax_entry(v) case v: errStr = #v; break;
			ax_entry(GL_INVALID_ENUM);
			ax_entry(GL_INVALID_VALUE);
			ax_entry(GL_INVALID_OPERATION);
			
			#if !AX_USE_OpenGLES
			ax_entry(GL_STACK_OVERFLOW);
			ax_entry(GL_STACK_UNDERFLOW);
			ax_entry(GL_OUT_OF_MEMORY);
			#endif
		#undef ax_entry
		}
		AX_LOG("GL Error {?} {?}\n  {?}", err, errStr, loc);
#endif
		throw axError_Undefined(AX_LOC);
		return false;
	}
	return true;
}

void axGLUtil::logExtensions() {
	axArray<axStrView> list;
	getExtensionList(list);

	axString s;
	s.append("glExtensions:\n");
	for (auto& e : list) {
		s.append("  ", e, "\n");
	}
	AX_LOG("{?}", s);
}

void axGLUtil::getExtensionList(axIArray<axStrView> & list) {
	GLint version = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &version);
	if (version < 3) {
		axStrView sv = getGLString(GL_EXTENSIONS);
		sv.getTokens(list, ' ');
	} else {
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);

		list.resize(n);
		for (GLuint i = 0; i < static_cast<GLuint>(n); i++) {
			list[i] = getGLStringi(GL_EXTENSIONS, i);
		}
	}
}

#endif //AX_USE_OPENGL
