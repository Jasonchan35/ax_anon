#pragma once

#if AX_USE_OPENGL

#include "../../texture/axTexture.h"
#include "axGLRenderer.h"

class axGLTexture2D : public axTexture2D {
	AX_RTTI_CLASS(axGLTexture2D, axTexture2D)
public:
	using Util = axGLUtil;

	axGLTexture2D(CreateDesc& desc);
	void onUploadRequest(axGLCommandDispatcher& dispatcher, UploadRequest& req);

	~axGLTexture2D();

	void bind() const { glBindTexture(kGLTarget, _tex);	}
	void unbind() const { glBindTexture(kGLTarget, 0); }

	const GLenum kGLTarget = GL_TEXTURE_2D;

	AX_INLINE const SamplerState& samplerState() const { return _samplerState; }
	AX_INLINE const axInt mipmapCount() const { return _mipmapCount; }

private:
	void _onUpdateRequest(const UploadRequest& req);
	axInt _compressedImageSize(axInt w, axInt h, axColorType colorType);

	GLuint	_tex = 0;

	struct Format {
		Format() = default;
		Format(axColorType colorType) { set(colorType); }
		void set(axColorType colorType);

		GLenum internalFormat{0};
		GLenum sourceFormat{0};
		GLenum elementType{0};
	};

	Format _format;
	SamplerState _samplerState;
	axInt _mipmapCount = 1;
};

AX_INLINE
axInt axGLTexture2D::_compressedImageSize(axInt w, axInt h, axColorType colorType) {
	auto blockSize = axColorUtil::bytesPerPixelBlock(_colorType);
	return ((w+3)/4) * ((h+3)/4) * blockSize;
}


#endif //AX_USE_OPENGL
