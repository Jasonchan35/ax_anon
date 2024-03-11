#pragma once

#include <ax_render/axRenderResource.h>

class axFontGlyphLayout;
class axFontProvider;
class axFontFace;
class axFont;

class axFontDesc {
public:
	axFontDesc() 
		: bold(false)
		, italic(false)
	{}

	axString		fontName;
	axInt			size {0};
	bool			bold : 1;
	bool			italic : 1;
};

class axFontGlyph {
public:
	axCharU	charCode = 0;
	axInt		glyphIndex = 0;
	axRect2f	outerRect	{0,0,0,0};
	axRect2f	vertexRect	{0,0,0,0};
	axRect2f	uv			{0,0,0,0};
	axVec2f		kerning		{0,0};
};

class axFont : public axRenderResource {
	AX_RTTI_CLASS(axFont, axRenderResource);
public:
	using Glyph = axFontGlyph;
	using Desc  = axFontDesc;

	axFont(const Desc& desc);

	const Desc&	desc() const { return _desc; }

			void getGlyphs(axIArray<Glyph>& outGlyphs, axStrView text) const;
			void getGlyphs(axIArray<Glyph>& outGlyphs, axStrViewU text) const;
	virtual void onGetGlyphs(axMutSpan<Glyph> glyphs, axStrViewU text) = 0;

	float lineHeight() const { return _lineHeight; }

protected:

	Desc _desc;
	axDict<axCharU, Glyph> _cachedGlyphs;

	float		_lineHeight = 0;
	Glyph		_missingGlyph;
	bool		_hasKerning : 1;
};

#define axFontStyleFlags_EnumList(E) \
	E(None,) \
	E(Underline,) \
	E(Strikethrough,) \
//----
ax_ENUM_CLASS(axFontStyleFlags, axUInt64)

namespace axTypeTraits {
	template<> struct defaultValue<axFontStyleFlags> { static void get(axFontStyleFlags& data) { data = axFontStyleFlags::None; } };
}

class axFontStyle : public axObject {
public:
	using Flags = axFontStyleFlags;

	axSPtr<axFont>	font;
	axColor			color {1,1,1};
	axColor			bgColor {0,0,0,0};
	axMargin2f		bgPadding {4};
	axColor			selectedColor {1,1,1};
	axColor			selectedBgColor {0,0,1};
	axEnum<Flags>	flags = Flags::None;

	ax_CLASS_BEGIN(axFontStyle, axObject)
		ax_FIELDS(	/* font, */
					color,
					bgColor,
					selectedColor,
					selectedBgColor,
					flags)
	ax_CLASS_END()
	AX_REFLECTION_IO_FUNC();

	axFontStyle() = default;
	axFontStyle(const axFontStyle& r) {
		operator=(r);
	}

	void operator=(const axFontStyle& r) {
		font = r.font;
		color = r.color;
		bgColor = r.bgColor;
		selectedColor = r.selectedColor;
		selectedBgColor = r.selectedBgColor;
		flags = r.flags;
	}
};


