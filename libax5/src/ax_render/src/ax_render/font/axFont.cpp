#include "axFont.h"

AX_REFLECTION_IO_FUNC_IMPL(, axFontStyle)

axFont::axFont(const Desc& desc) {
	_desc = desc;
	_hasKerning = false;
}

void axFont::getGlyphs(axIArray<Glyph>& outGlyphs, axStrView text) const {
	getGlyphs(outGlyphs, axTempStringU::s_convert(text));
}

void axFont::getGlyphs(axIArray<Glyph>& outGlyphs, axStrViewU text) const {
	outGlyphs.resizeNoCopy(text.size());
	ax_const_cast(this)->onGetGlyphs(outGlyphs.mutSpan(), text);
}
