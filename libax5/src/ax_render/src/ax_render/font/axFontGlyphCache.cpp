#include "axFont.h"
#include "axFontGlyphCache.h"
#include "axFontProvider.h"
#include "axFontManager.h"
#include "../axRenderContext.h"
#include "../axRenderRequest.h"
#include "../axRenderCommandDispatcher.h"

/*
http://clb.demon.fi/files/RectangleBinPack.pdf
Two-Dimensional Rectangle Bin Packing
- The Skyline Algorithms

*/

void axFontGlyphCache::create(axInt width, axInt height) {
	_packer.create(width, height);
	_image.create(width, height);

	axTexture2D_CreateDesc desc;
	desc.colorType	= _image.colorType();
	desc.size		= _image.size();
	desc.samplerState.filter = axTextureFilter::Point;

	_tex = axRenderer::s_instance()->createTexture2D(desc);
}

void axFontGlyphCache::onUpdateGpuResources(axRenderCommandDispatcher& dispatcher) {
	if (!_tex) return;

	axTexture2D_UploadRequest upload;
	upload.tex = _tex;
	upload.imageInfo = _image.info();
	upload.pixelData = _image.pixelData();
	dispatcher.uploadTexture2D(upload);
}

void axFontGlyphCache::onDebugRender(axRenderRequest& req) {
	if (!_tex) return;
	axVec2f drawPos(0, 200);
	req.ui.drawTexture(AX_LOC, _tex, drawPos);

	{
		axArray<axVec2f, 256> lines;

		auto skyline = _packer.skyline();
		auto n  = skyline.size();
		if (n < 2)
			return;

		auto* s = skyline.data();
		for (axInt i = 1; i < n; i++, s++) {
			lines.append(drawPos + axVec2f::s_cast(axVec2i(s[0].x, s[0].y)));
			lines.append(drawPos + axVec2f::s_cast(axVec2i(s[1].x, s[0].y)));
		}
		req.ui.drawLineStrip(AX_LOC, lines, axColor(0,1,0, 0.5f));
	}
}

bool axFontGlyphCache::onAddGlyph(axFont& font, Glyph& glyph, const axImageInfo& info, axByteSpan pixelData) {
	const axInt padding = 1;

	auto result = _packer.addRect(info.size + axVec2i(padding * 2));
	if (result.rect.x == 0) return false;

	auto uv = axRect2f::s_cast(result.rect);
	uv.x += padding;
	uv.y += padding;
	uv.w -= padding * 2;
	uv.h -= padding * 2;
	uv /= axVec2f::s_cast(_image.size());

	glyph.uv = uv;

	_image.subImage(result.rect.pos + axVec2i(padding), info, pixelData);
	_offset.x += info.size.x + 1;
	return true;
}
