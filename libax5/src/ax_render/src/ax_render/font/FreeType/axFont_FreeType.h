#pragma once

#include "axFontProvider_FreeType.h"

#if !AX_BUILD_ax_render
	#error "only include this file when building ax_render"
#endif

class axFont_FreeType : public axFont {
	AX_RTTI_CLASS(axFont_FreeType, axFont);
public:
	using FontFace = axFontFace_FreeType;

	~axFont_FreeType();

	axFont_FreeType(const Desc& desc, axFontFace_FreeType* face);

	virtual void onGetGlyphs(axMutSpan<Glyph> outGlyphs, axStrViewU text) override;

private:
	Glyph* _addGlyph(axCharU ch);
	axSPtr<FontFace> _fontFace;
	FT_Size _fontWithSize{nullptr};
};