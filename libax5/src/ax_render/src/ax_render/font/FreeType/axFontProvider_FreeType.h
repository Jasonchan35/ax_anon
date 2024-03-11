#pragma once

#include "axFontFile_FreeType.h"

#if !AX_BUILD_ax_render
	#error "only include this file when building ax_render"
#endif

class axFontProvider_FreeType : public axFontProvider {
	AX_RTTI_CLASS(axFontProvider_FreeType, axFontProvider);
public:
	using FontFile	 = axFontFile_FreeType;
	using FontFace	 = axFontFace;
	using FontFaceFT = axFontFace_FreeType;

	axFontProvider_FreeType();
	~axFontProvider_FreeType();
		
	virtual axFontFace* loadFontFile	(axStrView filename,				axStrView userDefineName) override;
	virtual axFontFace* loadFontMem	(axByteSpan mem, bool copyMem,	axStrView userDefineName) override;
	virtual axFontFace* loadFontMem	(axIByteArray && mem,				axStrView userDefineName) override;

	virtual axSPtr<Font> onCreateFont(const axFontDesc& desc) override;

	void onNewFile(FontFile* f);
	void onNewFace(FontFaceFT* f);
	void onNewFont(Font* f);

	FT_Library		lib() { return _lib; }

private:
	FT_MemoryRec_	_allocatorWrapper;
	FT_Library		_lib{nullptr};

	axSPtrArray<FontFile>	_fontFiles;
	axSPtrArray<FontFaceFT>	_fontFaces;
	axSPtrArray<Font>		_fonts;
};
