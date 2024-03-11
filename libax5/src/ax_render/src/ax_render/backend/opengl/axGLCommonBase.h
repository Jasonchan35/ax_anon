#pragma once

#if AX_USE_OPENGL

#include "../../vertex/axRenderVertexDesc.h"
#include "../../shader/axRenderState.h"
#include "../../axRenderCommonBase.h"

#if TRACY_ENABLE
#include <tracy/TracyOpenGL.hpp>
#endif

class axGLRenderer;
class axGLTexture2D;
class axGLMaterial;
class axGLCommandDispatcher;
class axGLRenderContext_Base;
class axGLRenderContext;

class axGLUtil : public axRenderCommonBase {
public:
	static axGLRenderer*	renderer();
	static bool checkGLError(const axSourceLoc& loc);

	static void			logExtensions();
	static void			getExtensionList(axIArray<axStrView> & list);

	static axRenderDataType getAxDataType		(GLenum type);

	static GLenum	getGLPrimitiveTopology	(axRenderPrimitiveType t);
	static GLenum	getGLIndexType			(axRenderIndexType t);
	static GLenum	getGLDataType			(axRenderDataType v);

	static GLenum	getGLBlendOp			(axRenderState_BlendOp v);
	static GLenum	getGLBlendFactor		(axRenderState_BlendFactor v);

	static GLenum	getGLDepthTestOp		(axRenderState_DepthTestOp v);

	static GLint	getGLTextureMinFilter	(axTextureFilter v, axInt mipmapCount);
	static GLint	getGLTextureMagFilter	(axTextureFilter v);
	static GLint	getGLTextureWrap		(axTextureWrap v);

	static GLsizei	castGLsizei	(axInt v)	{ return ax_safe_static_cast<GLsizei>(v); }
	static GLuint	castGLuint	(axInt v)	{ return ax_safe_static_cast<GLuint >(v); }
	static GLint	castGLint	(axInt v)	{ return ax_safe_static_cast<GLint  >(v); }

	static axStrView	getGLString	(GLenum name)			{ auto* sz = (const char*)glGetString(name);     return axStrView_c_str(sz); }
	static axStrView	getGLStringi(GLenum name, GLuint i)	{ auto* sz = (const char*)glGetStringi(name, i); return axStrView_c_str(sz);  }

	static void			getGLValue	(GLenum name, bool&    value) { GLboolean v = 0; glGetBooleanv(name, &v); value = v; }
	static void			getGLValue	(GLenum name, GLint&   value) { glGetIntegerv  (name, &value); }
	static void			getGLValue	(GLenum name, GLint64& value) { glGetInteger64v(name, &value); }
	static void			getGLValue	(GLenum name, float&   value) { glGetFloatv    (name, &value); }
	static void			getGLValue	(GLenum name, double&  value) { glGetDoublev   (name, &value); }

	static void			getGLValuei	(GLenum name, GLuint i, bool&    value) { GLboolean v = 0; glGetBooleani_v(name, i, &v); value = v; }
	static void			getGLValuei	(GLenum name, GLuint i, GLint&   value) { glGetIntegeri_v  (name, i, &value); }
	static void			getGLValuei	(GLenum name, GLuint i, GLint64& value) { glGetInteger64i_v(name, i, &value); }
	static void			getGLValuei	(GLenum name, GLuint i, float&   value) { glGetFloati_v    (name, i, &value); }
	static void			getGLValuei	(GLenum name, GLuint i, double&  value) { glGetDoublei_v   (name, i, &value); }

	template<class T> static T	getGLValue_ (GLenum name)			{ T v; getGLValue(name, v); return v; }
	template<class T> static T	getGLValuei_(GLenum name, GLuint i)	{ T v; getGLValue(name, v); return v; }
};

inline
GLenum axGLUtil::getGLPrimitiveTopology(axRenderPrimitiveType t) {
	using SRC = axRenderPrimitiveType;
	switch (t) {
		case SRC::Points:			return GL_POINTS;
		case SRC::Lines:			return GL_LINES;
		case SRC::Triangles:		return GL_TRIANGLES;
		default: AX_ASSERT(false);	return GL_NONE;
	}
}

inline
GLenum axGLUtil::getGLIndexType(axRenderIndexType t) {
	using SRC = axRenderIndexType;
	switch (t) {
		case SRC::UInt16:			return GL_UNSIGNED_SHORT;
		case SRC::UInt32:			return GL_UNSIGNED_INT;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLenum axGLUtil::getGLDataType(axRenderDataType v) {
	using SRC = axRenderDataType;
	switch (v) {
		case SRC::UInt8:		return GL_UNSIGNED_BYTE;
		case SRC::UInt8x2:		return GL_UNSIGNED_BYTE;
		case SRC::UInt8x3:		return GL_UNSIGNED_BYTE;
		case SRC::UInt8x4:		return GL_UNSIGNED_BYTE;
	//---
		case SRC::Float16:		return GL_HALF_FLOAT;
		case SRC::Float16x2:	return GL_HALF_FLOAT;
		case SRC::Float16x3:	return GL_HALF_FLOAT;
		case SRC::Float16x4:	return GL_HALF_FLOAT;
	//---
		case SRC::Float32:		return  GL_FLOAT;
		case SRC::Float32x2:	return  GL_FLOAT;
		case SRC::Float32x3:	return  GL_FLOAT;
		case SRC::Float32x4:	return  GL_FLOAT;
	//---
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLenum axGLUtil::getGLBlendOp(axRenderState_BlendOp v) {
	using SRC = axRenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return GL_FUNC_ADD;
		case SRC::Sub:		return GL_FUNC_SUBTRACT;
		case SRC::RevSub:	return GL_FUNC_REVERSE_SUBTRACT;
		case SRC::Min:		return GL_MIN;
		case SRC::Max:		return GL_MAX;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLenum axGLUtil::getGLBlendFactor(axRenderState_BlendFactor v) {
	using SRC = axRenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:					return GL_ZERO;
		case SRC::One:					return GL_ONE;
	//---
		case SRC::SrcAlpha:				return GL_SRC_ALPHA;
		case SRC::DstAlpha:				return GL_DST_ALPHA;
		case SRC::SrcColor:				return GL_SRC_COLOR;
		case SRC::DstColor:				return GL_DST_COLOR;
		case SRC::ConstColor:			return GL_CONSTANT_COLOR;
//		case SRC::ConstAlpha:			return GL_CONSTANT_ALPHA;
	//---
		case SRC::OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case SRC::OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
		case SRC::OneMinusDstAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case SRC::OneMinusDstColor:		return GL_ONE_MINUS_DST_COLOR;
		case SRC::OneMinusConstColor:	return GL_ONE_MINUS_CONSTANT_COLOR;
//		case SRC::OneMinusConstAlpha:	return GL_ONE_MINUS_CONSTANT_ALPHA;
	//---
		case SRC::SrcAlphaSaturate:		return GL_SRC_ALPHA_SATURATE;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLenum axGLUtil::getGLDepthTestOp(axRenderState_DepthTestOp v) {
	using SRC = axRenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return GL_ALWAYS;
		case SRC::Less:			return GL_LESS;
		case SRC::Equal:		return GL_EQUAL;
		case SRC::LessEqual:	return GL_LEQUAL;
		case SRC::Greater:		return GL_GREATER;
		case SRC::GreaterEqual:	return GL_GEQUAL;
		case SRC::NotEqual:		return GL_NOTEQUAL;
		case SRC::Never:		return GL_NEVER;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLint axGLUtil::getGLTextureMinFilter(axTextureFilter v, axInt mipmapCount) {
	bool m = mipmapCount > 1;

	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return m ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		case SRC::Linear:		return m ? GL_NEAREST_MIPMAP_LINEAR  : GL_NEAREST;
		case SRC::Bilinear:		return m ? GL_LINEAR_MIPMAP_NEAREST  : GL_LINEAR;
		case SRC::Trilinear:	return m ? GL_LINEAR_MIPMAP_LINEAR   : GL_LINEAR;
		case SRC::Anisotropic:	return GL_LINEAR;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLint axGLUtil::getGLTextureMagFilter(axTextureFilter v) {
	using SRC = axTextureFilter;
	switch (v) {
		case SRC::Point:		return GL_NEAREST;
		case SRC::Linear:		return GL_NEAREST;
		case SRC::Bilinear:		return GL_LINEAR;
		case SRC::Trilinear:	return GL_LINEAR;
		case SRC::Anisotropic:	return GL_LINEAR;
		default: throw axError_Undefined(AX_LOC);
	}
}

inline
GLint axGLUtil::getGLTextureWrap(axTextureWrap v) {
	using SRC = axTextureWrap;
	switch (v) {
		case SRC::Repeat:		return GL_REPEAT;
		case SRC::Clamp:		return GL_CLAMP_TO_EDGE;
		case SRC::Mirror:		return GL_MIRRORED_REPEAT;
#if !AX_USE_OpenGLES
		case SRC::MirrorOnce:	return GL_MIRROR_CLAMP_TO_EDGE;
#endif
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_OPENGL
