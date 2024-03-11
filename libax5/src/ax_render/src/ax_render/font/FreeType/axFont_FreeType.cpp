#include "axFont_FreeType.h"
#include "axFontFace_FreeType.h"

axFont_FreeType::~axFont_FreeType() {
	if (!_fontFace) return;

	ax_LOCK(_fontFace->_mutex);
	if (_fontWithSize) {
		FT_Done_Size(_fontWithSize);
		_fontWithSize = nullptr;
	}
}

axFont_FreeType::axFont_FreeType(const Desc& desc, axFontFace_FreeType* fontFace) 
	: Base(desc)
{
	_fontFace.ref(fontFace);

	ax_LOCK(_fontFace->_mutex);
	auto face = fontFace->_face;

	FT_Error err;
	err = FT_New_Size(face, &_fontWithSize);
	if (err) throw axError_Undefined(AX_LOC);

	err = FT_Activate_Size(_fontWithSize);
	if (err) throw axError_Undefined(AX_LOC);

	//auto pts = 64;
	//auto dpi = 300;
	//err = FT_Set_Char_Size(face, 0, 16 * pts, dpi, dpi);
	//if (err) throw axError_Undefined(AX_LOC);
		
	err = FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(desc.size));
	if (err) throw axError_Undefined(AX_LOC);

	_lineHeight = _fontWithSize->metrics.height / 64.0f;

	axRect2f rc(0, 0, _fontWithSize->metrics.max_advance / 64.0f, _lineHeight);
	_missingGlyph.outerRect  = rc;
	_missingGlyph.vertexRect = rc;

	_hasKerning = FT_HAS_KERNING(_fontFace->_face) != 0;
}

void axFont_FreeType::onGetGlyphs(axMutSpan<Glyph> outGlyphs, axStrViewU text) {
	if (outGlyphs.size() != text.size()) throw axError_Undefined(AX_LOC);

	ax_LOCK(_fontFace->_mutex);

	FT_Error err;
	err = FT_Activate_Size(_fontWithSize);
	if (err) throw axError_Undefined(AX_LOC);

	auto* face = _fontFace->_face;
	err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (err) throw axError_Undefined(AX_LOC);

	axInt lastGlyphIndex = 0;
	auto* ch = text.begin();

	bool hasKerning = FT_HAS_KERNING(face);

	for (auto& dst : outGlyphs) {
		auto* src = _cachedGlyphs.find(*ch);
		if (!src) {
			src = _addGlyph(*ch);
		}
		if (!src) {
			//throw axError_Undefined(AX_LOC);
			return;
		}

		dst = *src;

		if (!hasKerning) {
			dst.kerning.set(0, 0);

		} else if (lastGlyphIndex) {
			FT_Vector  kerning;
			FT_Get_Kerning(face, static_cast<FT_UInt>(lastGlyphIndex), static_cast<FT_UInt>(src->glyphIndex), FT_KERNING_DEFAULT, &kerning);
			dst.kerning = axVec2f(static_cast<float>(kerning.x), static_cast<float>(kerning.y)) / 64.0f;
		}

		lastGlyphIndex = src->glyphIndex;
		ch++;
	}
	AX_ASSERT(ch == text.end());
}

axFontGlyph* axFont_FreeType::_addGlyph(axCharU ch) {
	ax_LOCK(_fontFace->_mutex);

	auto glyph = _missingGlyph;
	auto* face = _fontFace->_face;

	glyph.charCode   = ch;

	if (ch == '\n' || ch == '\t') {
		glyph.vertexRect.set(0,0,0,0);
	} else {
		auto glyphIndex = FT_Get_Char_Index(face, ch);
		if (!glyphIndex) {
			return &_missingGlyph;
		}

		glyph.glyphIndex	= glyphIndex;

		FT_Error err;
		err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_FORCE_AUTOHINT);
		if (err) throw axError_Undefined(AX_LOC);

		FT_GlyphSlot  slot = face->glyph;

		err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL); /* render mode */
		if (err) throw axError_Undefined(AX_LOC);

		if (slot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
			throw axError_Undefined(AX_LOC);
		}

		auto& bmp = slot->bitmap;
		auto* pixels	= reinterpret_cast<axColorRb*>(bmp.buffer);

		axImageInfo info;
		info.size.set(bmp.width, bmp.rows);
		info.strideInBytes = bmp.pitch;
		info.colorType = axColorType::Rb;
		
		if (bmp.rows == 0) {
			pixels = nullptr;
			info.strideInBytes = 0;
		}else if (info.strideInBytes < 0) { //upside down
			pixels = pixels + (bmp.rows-1) * info.strideInBytes;
			info.strideInBytes = -info.strideInBytes;
		}

		float ascenter = _fontWithSize->metrics.ascender / 64.0f;

		glyph.vertexRect.set(	slot->bitmap_left / 64.0f, 
								ascenter - slot->bitmap_top,
								static_cast<float>(bmp.width),
								static_cast<float>(bmp.rows));

		glyph.outerRect.set(0, 0, slot->advance.x / 64.0f, _lineHeight);

		axByteMutSpan pixelData(reinterpret_cast<axByte*>(pixels), bmp.rows * info.strideInBytes);
		if (!axFontManager::s_instance()->glypchCache()->onAddGlyph(*this, glyph, info, pixelData)) {
			return nullptr;
		}
	}

	auto& o = _cachedGlyphs.add(ch, glyph);
	return &o;
}

/*
void axFont_FreeType::onGetKerning(axMutSpan<LayoutGlyph> view) {
	if (view.size() <= 0) return;

	auto lock = ax_lock(_fontFace->_mutex);

	auto err = FT_Activate_Size(_fontWithSize);
	if (err) throw axError_Undefined(AX_LOC);

	auto* dst = view.data();
	auto* end = dst + view.size();

	dst->kerning.set(0, 0);

	axInt prevGlyphIndex = 0;
	axInt curGlyphIndex = 0;
	if (auto* c = _cached.find(dst->character)) {
		prevGlyphIndex = curGlyphIndex = c->glyphIndex;
	}
	dst++;

	auto face = _fontFace->_face;

	FT_Vector  delta;
	for (; dst < end; dst++) {

		if (auto* c = _cached.find(dst->character)) {
			curGlyphIndex = c->glyphIndex;
		}

		FT_Get_Kerning(face, prevGlyphIndex, curGlyphIndex, FT_KERNING_DEFAULT, &delta);
		dst->kerning = axVec2f(delta.x, delta.y) / 64.0;

		prevGlyphIndex = curGlyphIndex;
	}	
}
*/
