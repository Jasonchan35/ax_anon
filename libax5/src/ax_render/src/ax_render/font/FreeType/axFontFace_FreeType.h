#pragma once

#include <ax_render/font/axFontProvider.h>
#include <ax_render/font/axFontManager.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

#if !AX_BUILD_ax_render
	#error "only include this file when building ax_render"
#endif

class axFontFile_FreeType;
class axFontProvider_FreeType;

class axFontFace_FreeType : public axFontFace {
public:
	using FontFile = axFontFile_FreeType;

	axFontFace_FreeType(FontFile* fontFile, axStrView userDefineName);
	~axFontFace_FreeType();

	axSPtr<axFont> createFont(const axFontDesc& desc);

	axMutex _mutex;

	axSPtr<FontFile> _fontFile;
	FT_Face _face {nullptr};
};
