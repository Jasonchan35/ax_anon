#include <ax_ui/text/axUITextBlock.h>
#include <ax_ui/base/axUIDefaultAssets.h>

axUITextSpan::axUITextSpan(axUITextSpan && r) {
	style = ax_move(r.style);
	_text = ax_move(r._text);
}

axUITextSpan::axUITextSpan(axStrViewU text_, const axFontStyle* style_) {
	setText(text_);
	style.ref(style_);
}

void axUITextSpan::setText(axStrView text) {
	_text.setUtf(text);
}

void axUITextSpan::setText(axStrViewU text) {
	_text.set(text);
}

axUITextBlock::axUITextBlock()
	: _needToLayout(false)
{
}

void axUITextBlock::addText(axStrViewU text, const Style* style) {
	_textSpans.emplaceBack(text, style);
	_needToLayout = true;
}

void axUITextBlock::setText(axStrViewU text, const Style* style) {
	clear();
	addText(text, style);
}

axInt axUITextBlock::totalCharCount() const {
	axInt o = 0;
	for (auto& ts : _textSpans) {
		o += ts.text().size();
	}
	return o;
}

void axUITextBlock::clear() {
	_needToLayout = true;
	_textSpans.clear();
	_lines.clear();
}

bool axUITextBlock::layout() {
	if (!_needToLayout) return false;
	_doLayout();
	return true;
}

void axUITextBlock::_doLayout() {
	_lines.clear();
	_glyphs.clear();
	_boundingBox.clear();

	auto& defaultStyle = axUIDefaultAssets::s_instance()->fontStyles.std;

	auto charCount = totalCharCount();
	_glyphs.resize(charCount);
	auto remainGlyphs = _glyphs.mutSpan();

	// update glyph cache and get kerning
	for (auto& ts : _textSpans) {
		auto text = ts.text();
		if (!text) continue;

		auto* style = ts.style.ptr();
		if (!style) style = defaultStyle.ptr();
		if (!style) continue;

		auto* font = ax_const_cast(style->font.ptr());
		if (!font) continue;

		auto spanGlyphs = remainGlyphs.slice(0, text.size());
		font->onGetGlyphs(spanGlyphs, text);
		remainGlyphs = remainGlyphs.sliceFrom(spanGlyphs.size());
	}
	_glyphs.decSize(remainGlyphs.size());

	// do layout

	float maxHeightInThisLine = 0;
	Vec2 pos(0, 0);

	axInt glyphsCount = _glyphs.size();

	axInt lineStart = 0;
	for (axInt i = 0; i < glyphsCount; i++) {
		auto& g = _glyphs[i];
		ax_max_it(maxHeightInThisLine, g.outerRect.h);

		switch (g.charCode) {
			case '\n': {
				auto& line = _lines.emplaceBack();
				line.glyphs = _glyphs.slice(lineStart, i - lineStart);
				line.rect.set(0, pos.y, pos.x, maxHeightInThisLine);
				lineStart = i;

				pos.x = 0;
				pos.y += ax_ceil(maxHeightInThisLine * 1.08f);
				maxHeightInThisLine = 0;
			} continue;

			case '\t':
				continue;
		}

		if (g.glyphIndex == 0)
			continue;
		
		g.outerRect.pos  += pos;
		g.vertexRect.pos += pos;

		_boundingBox.include(g.outerRect.pos);
		_boundingBox.include(g.outerRect.xMaxYMax());

		pos.x += g.outerRect.w + _letterSpacing;
	}

	if (glyphsCount - lineStart > 0) {
		auto& line = _lines.emplaceBack();
		line.glyphs = _glyphs.slice(lineStart, glyphsCount - lineStart);
		line.rect.set(0, pos.y, pos.x, maxHeightInThisLine);
	}
}

