#pragma once

#include "axUITextBlock.h"

class axUITextMesh {
public:
	using Glyph = axFontGlyph;
	using Style = axFontStyle;
	using Color = axColor;
	using Mat4  = axMat4f;
	using Rect2 = axRect2f;
	using Vec2  = axVec2f;
	using Align = axUIAlign;
	using Margin2 = axMargin2f;

	void addText(axStrView text, const Style* style = nullptr);
	void setText(axStrView text, const Style* style = nullptr);

	void addText(axStrViewU text, const Style* style = nullptr);
	void setText(axStrViewU text, const Style* style = nullptr);

	void clear();

	void render(axRenderRequest& req, const Vec2& pos);
	void renderInRect(axRenderRequest& req, const Rect2& rect);

	void render(axRenderRequest& req, const Mat4& mat);
	void render(axRenderRequest& req);

	void setAlign(const Align& v) { _block.setAlign(v); }

	void setColor				(const Color& c) { _color = c; }
	void setBgColor				(const Color& c) { _bgColor = c; }
	void setSelectionStart		(axInt i);
	void setSelectionEnd		(axInt i);
	void setSelectionRange		(axInt start, axInt end) { setSelectionStart(start); setSelectionEnd(end); }

	const axUITextBlock& block() { updateMesh(); return _block; }

	void updateMesh();
	const axBBox2f&	boundingBox();

	void onMeasureLayout(axUIMeasureRequest& m);

	void setIsInlineDraw(bool b);

private:
	void calcLayout(axIArray<Glyph>& glyphs);

friend class axUIFontGlyphCache;
protected:

	axUITextBlock		_block;
	axRenderMesh		_textMesh;
	axRenderMesh		_bgMesh;
	Color				_color {1,1,1,1};
	Color				_bgColor {0,0,0,0};
	axInt				_selectionStart = 0;
	axInt				_selectionEnd   = 0;

	bool _meshDirty = false;
};
