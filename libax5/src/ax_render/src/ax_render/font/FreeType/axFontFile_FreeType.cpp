#include "axFontFile_FreeType.h"
#include "axFont_FreeType.h"

axFontFile_FreeType::axFontFile_FreeType(axFontProvider_FreeType* provider) 
	: _provider(provider)
{
	provider->onNewFile(this);
}

axFontFace* axFontFile_FreeType::loadFile(axStrView filename, axStrView userDefineName) {
	_mapping.openFile(filename);
	return loadMem(_mapping, false, userDefineName);
}

axFontFace* axFontFile_FreeType::loadMem(axByteSpan mem, bool copyMem, axStrView userDefineName) {
	if (copyMem) {
		_memBuf.set(mem);
		_memSpan = _memBuf;
	}else{
		_memSpan = mem;
	}
	return _doLoad(userDefineName);
}

axFontFace* axFontFile_FreeType::loadMem(axIByteArray && mem, axStrView userDefineName) {
	_memBuf.move(mem);
	_memSpan = _memBuf;
	return _doLoad(userDefineName);
}

axFontFace* axFontFile_FreeType::_doLoad(axStrView userDefineName) {
	return ax_new axFontFace_FreeType(this, userDefineName);
}
