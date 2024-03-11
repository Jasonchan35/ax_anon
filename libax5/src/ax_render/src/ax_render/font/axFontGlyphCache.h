#pragma once

#include <ax_render/texture/axRectBinPacker_Skyline.h>
#include <ax_render/texture/axTexture.h>
#include <ax_render/image/axImage.h>

class axFontManager;
class axFontGlyph;
class axFont;
class axRenderContext;
class axRenderRequest;

class axFontGlyphCache : public axNonCopyable {
public:
	using Texture2D = axTexture2D;
	using Glyph = axFontGlyph;

	void create(axInt width, axInt height);
	Texture2D*	getTexture() { return _tex; }

	void onUpdateGpuResources(axRenderCommandDispatcher& dispatcher);
	void onDebugRender(axRenderRequest& req);

	bool onAddGlyph(axFont& font, Glyph& glyph, const axImageInfo& info, axByteSpan pixelData);

private:
	axRectBinPacker_Skyline	_packer;

	axSPtr<Texture2D>	_tex;
//	axRect2i			_dirtyRect {0,0,0,0};

	axImageRb			_image;
	axVec2i				_offset {0,0};

};
