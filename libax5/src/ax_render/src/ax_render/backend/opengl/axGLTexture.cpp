#if AX_USE_OPENGL

#include "axGLTexture.h"
#include "axGLRenderContext.h"
#include "axGLRenderer.h"
#include "axGLCommonBase.h"

axGLTexture2D::axGLTexture2D(CreateDesc& desc) 
	: Base(desc)
{
	glGenTextures(1, &_tex);
	if (!_tex) {
		Util::checkGLError(AX_LOC);
		throw axError_Undefined(AX_LOC);
	}

	_format.set(desc.colorType);
	_samplerState = desc.samplerState;
	_mipmapCount = desc.mipmapCount;

	glBindTexture(kGLTarget, _tex);

	auto w = static_cast<GLsizei>(desc.size.x); 
	auto h = static_cast<GLsizei>(desc.size.y);

	if (axColorUtil::isCompressedType(desc.colorType)) {
		auto imageSize = static_cast<GLsizei>(_compressedImageSize(w, h, desc.colorType));
		glCompressedTexImage2D(kGLTarget, 0, _format.internalFormat, w, h, 0, imageSize, nullptr);

	} else {
		glTexImage2D(kGLTarget, 0, (GLint)_format.internalFormat, w, h, 0, _format.sourceFormat, _format.elementType, nullptr);
	}
	/*
	// require OpenGL 4.2
	int mipmap = 1;
	glTexStorage2D(	_target,
					static_cast<GLsizei>(mipmap), 
					_format.internalFormat, 
					static_cast<GLsizei>(desc.size.x), 
					static_cast<GLsizei>(desc.size.y));
	*/

	if (desc.uploadRequest) {
		_onUpdateRequest(*desc.uploadRequest);
	}
	glBindTexture(kGLTarget, 0);
}

void axGLTexture2D::onUploadRequest(axGLCommandDispatcher& dispatcher, UploadRequest& req) {
	AX_ASSERT(req.tex.ptr() == this);
	if (!_tex) {
		throw axError_Undefined(AX_LOC);
	}
	glBindTexture(kGLTarget, _tex);
	_onUpdateRequest(req);
	glBindTexture(kGLTarget, 0);
}

axGLTexture2D::~axGLTexture2D() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

void axGLTexture2D::_onUpdateRequest(const UploadRequest& req) {
	auto& info = req.imageInfo;

	if (info.colorType != _colorType) {
		throw axError_Undefined(AX_LOC);
	}

	if (req.offset.x < 0 || req.offset.y < 0) {
		throw axError_Undefined(AX_LOC);
	}

	//make sure within texture area
	auto s = ax_clamp(info.size, axVec2i(0,0), size() - req.offset);
	if (s != info.size) {
		throw axError_Undefined(AX_LOC);
	}

	auto x = static_cast<GLint>(req.offset.x);
	auto y = static_cast<GLint>(req.offset.y);
	auto w = static_cast<GLsizei>(s.x);
	auto h = static_cast<GLsizei>(s.y);

	if (axColorUtil::isCompressedType(_colorType)) {
		auto imageSize = static_cast<GLsizei>(_compressedImageSize(w, h, _colorType));
		if (req.pixelData.size() < imageSize) {
			throw axError_Undefined(AX_LOC);
		}
		glCompressedTexSubImage2D(kGLTarget, 0, x, y, w, h, _format.internalFormat, imageSize, req.pixelData.data());

	} else {
		if (info.size.x * info.pixelSizeInBytes() != info.strideInBytes) {
			throw axError_Undefined(AX_LOC);
		}

		axInt pixelSizeInBytes = axColorUtil::pixelSizeInBytes(info.colorType);
		axInt expectedDataSize = info.strideInBytes * info.size.y;
		if (req.pixelData.size() < expectedDataSize) {
			throw axError_Undefined(AX_LOC);
		}

		axInt unpack_align = 4;
		if (pixelSizeInBytes % 4) unpack_align = 1;

		if (unpack_align != 4) {
			glPixelStorei( GL_UNPACK_ALIGNMENT, static_cast<GLint>(unpack_align) );
		}

		glTexSubImage2D(kGLTarget, 0, x, y, w, h, _format.sourceFormat, _format.elementType, req.pixelData.data());

		if (unpack_align != 4) {
			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
		}
	}
}

void axGLTexture2D::Format::set(axColorType colorType) {
	switch (colorType) {
		case axColorType::RGBAb:	{ internalFormat = GL_RGBA8;	sourceFormat = GL_RGBA;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::RGBb:		{ internalFormat = GL_RGB8;		sourceFormat = GL_RGB;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::RGb:		{ internalFormat = GL_RG8;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::Rb:		{ internalFormat = GL_R8;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::Ab:		{ internalFormat = GL_R8;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::Lb:		{ internalFormat = GL_R8;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_BYTE;	}break;
		case axColorType::LAb:		{ internalFormat = GL_RG8;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_BYTE;	}break;
	#if !AX_USE_OpenGLES
		case axColorType::BC1:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;	sourceFormat = 0; elementType = 0;	}break;
		case axColorType::BC2:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;	sourceFormat = 0; elementType = 0;	}break;
		case axColorType::BC3:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;	sourceFormat = 0; elementType = 0;	}break;
		case axColorType::BC7:		{ internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM;		sourceFormat = 0; elementType = 0;	}break;
	#endif		
		default: throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_USE_OPENGL
