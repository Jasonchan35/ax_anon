#pragma once

#include "axFontFace_FreeType.h"

class axFontFile_FreeType : public axObject {
	AX_RTTI_CLASS(axFontProvider_FreeType, axObject)
public:
	axFontFile_FreeType(axFontProvider_FreeType* provider);

	axFontFace* loadFile	(axStrView filename,				axStrView userDefineName);
	axFontFace* loadMem	(axByteSpan mem, bool copyMem,	axStrView userDefineName);
	axFontFace* loadMem	(axIByteArray && mem,				axStrView userDefineName);

	axSPtr<axFont> createFont(const axFontDesc& desc);

	axFontFace* _doLoad	(axStrView userDefineName);

	axFontProvider_FreeType* _provider {nullptr};
	axMemMap	_mapping;
	axByteArray		_memBuf;
	axByteSpan		_memSpan;
};
