#include "axFontFace_FreeType.h"
#include "axFont_FreeType.h"

axFontFace_FreeType::axFontFace_FreeType(FontFile* fontFile, axStrView userDefineName) {
	ax_LOCK(_mutex);

	_fontFile.ref(fontFile);
	auto* provider = _fontFile->_provider;
	auto& memSpan = _fontFile->_memSpan;

	auto err = FT_New_Memory_Face(provider->lib(), memSpan.data(), static_cast<FT_Long>(memSpan.size()), 0, &_face);
	if (err) {
		throw axError_Undefined(AX_LOC);
	}

	_family     = axStrView_c_str(_face->family_name);
	_styleName  = axStrView_c_str(_face->style_name);

	if (_styleName == "Regular") {
		_styleName = "";
	}

	if (userDefineName) {
		_name = userDefineName;
	}else{
		_name = _family;
		if (_styleName) {
			_name.append("-", _styleName);
		}
	}

	provider->onNewFace(this);
}

axFontFace_FreeType::~axFontFace_FreeType() {
	ax_LOCK(_mutex);
	if (_face) {
	    FT_Done_Face(_face);
		_face = nullptr;
	}
}

axSPtr<axFont> axFontFace_FreeType::createFont(const axFontDesc& desc) {
	ax_LOCK(_mutex);
	if (!_face) {
		throw axError_Undefined(AX_LOC);
	}
	auto outFont = axSPtr_new(axFont_FreeType(desc, this));
	_fontFile->_provider->onNewFont(outFont);
	return outFont.ptr();
}
