#pragma once

#include "axFont.h"
#include "axFontGlyphCache.h"
#include "axFontProvider.h"
#include <ax_render/texture/axTexture.h>

class axUITextMesh;
class axUITextBlock;

class axFontFace : public axObject {
	AX_RTTI_CLASS(axFontFace, axObject)
public:
	axStrView name		() const { return _name; }
	axStrView family	() const { return _family; }
	axStrView styleName	() const { return _styleName; }

protected:
	axString _name;
	axString _family;
	axString _styleName;
};

class axFontManager : public axNonCopyable {
	using This = axFontManager;
public:
	static axFontManager* s_instance();
	using Font = axFont;
	using FontDesc = axFontDesc;
	using FontFace = axFontFace;
	using Texture2D = axTexture2D;

	axFontManager();
	~axFontManager();

	FontFace* loadFontFile	(axStrView filename, axStrView userDefineName = axStrView::s_empty());

	axSPtr<Font> createFont	(const FontDesc& desc);
	axSPtr<Font> createFont	(axStrView face, axInt size, bool bold = false, bool italic = false);

	axAllocator*	allocator()			{ return _allocator; }
	Texture2D*		getGlyphTexture()	{ return _glyphCache.getTexture(); };

	void onUpdateGpuResources(axRenderCommandDispatcher& dispatcher);
	void onDebugRender(axRenderRequest& req);

	axFontGlyphCache* glypchCache() { return &_glyphCache; }

private:
	axAllocator*			_allocator{nullptr};
	axSPtr<axFontProvider>	_provider;
	axFontGlyphCache		_glyphCache;
};