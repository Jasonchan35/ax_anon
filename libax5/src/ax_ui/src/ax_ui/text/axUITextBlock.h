#pragma once

#include <ax_ui/text/axUITextStyle.h>

class axUIFontGlyphCache;

// same axUITextSpan share the same Style
class axUITextSpan {
public:
	using Style = axFontStyle;

	axUITextSpan() = default;
	axUITextSpan(axUITextSpan && r);
	axUITextSpan(axStrViewU text_, const Style* style_);

	void setText(axStrViewU text);
	void setText(axStrView text);

	axStrViewU text() const { return _text; }

	axSPtr<const Style> style;

private:
	axStringU	_text;
};

// axUITextBlock contains multiple span in different styles
class axUITextBlock {
public:
	using Span = axUITextSpan;
	using Glyph = axFontGlyph;
	using Style = axFontStyle;
	using Rect2 = axRect2f;
	using Vec2  = axVec2f;
	using Align = axUIAlign;

	axUITextBlock();

	void	addText(axStrViewU text, const Style* style = nullptr);
	void	setText(axStrViewU text, const Style* style = nullptr);

	axInt	totalCharCount() const;
	void	clear();
	bool	layout();

	const axBBox2f&	boundingBox() const { return _boundingBox; }

	class Line {
	public:
		Rect2			rect;
		axSpan<Glyph>	glyphs;
	};

	axSpan<Line>	lines	() const { return _lines;  }
	axSpan<Glyph>	glyphs	() const { return _glyphs; }
	
	void setAlign(const Align& v) { _align = v; }

	const Align&	align	() const { return _align; }

private:
	void _doLayout();

	axArray<Span, 1>	_textSpans;
	axArray<Glyph, 64>	_glyphs;
	axArray<Line, 1>	_lines;

	axRect2f		_rect{0,0,0,0};
	Align			_align;
	float			_letterSpacing = 0;

	axBBox2f		_boundingBox;

	bool	_needToLayout : 1;
};

