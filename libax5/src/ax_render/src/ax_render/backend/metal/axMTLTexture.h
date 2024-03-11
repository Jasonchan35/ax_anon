#pragma once

#if AX_USE_METAL

#include "../../texture/axTexture.h"
#include "axMTLRenderer.h"

class axMTLTexture2D : public axTexture2D {
	AX_RTTI_CLASS(axMTLTexture2D, axTexture2D)
public:
	using Util = axMTLUtil;

	axMTLTexture2D(CreateDesc& desc);
	void onUploadRequest(axMTLCommandDispatcher& dispatcher, UploadRequest& req);

	~axMTLTexture2D();

	void bind() const;
	void unbind() const {
		ax_Metal_TODO
	}

	id<MTLTexture>		_mtlTexture = nil;
	id<MTLSamplerState> _samplerState = nil;

private:
	void _onUpdateRequest(const UploadRequest& req);
};

#endif //AX_USE_METAL
