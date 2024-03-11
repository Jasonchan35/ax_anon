#pragma once

#include "axFont.h"

class axFontProvider : public axObject {
	AX_RTTI_CLASS(axFontProvider, axObject);
public:
	using Font = axFont;

			axSPtr<Font> createFont  (const axFontDesc& desc);
	virtual axSPtr<Font> onCreateFont(const axFontDesc& desc) = 0;

	virtual axFontFace* loadFontFile	(axStrView filename,				axStrView userDefineName) = 0;
	virtual axFontFace* loadFontMem	(axByteSpan mem, bool copyMem,	axStrView userDefineName) = 0;
	virtual axFontFace* loadFontMem	(axIByteArray && mem,				axStrView userDefineName) = 0;
	
private:
};