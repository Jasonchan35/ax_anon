#include "axFontProvider_FreeType.h"
#include "axFont_FreeType.h"

void* axFontProvider_FreeType_MemAlloc(FT_Memory memory, long  size) {
	if (size <= 0) {
		AX_ASSERT(false);
		return nullptr;
	}
	void* p = ::malloc((size_t)size);
	if (p) {
		::memset(p, 0, (size_t)size);
	}
	return p;
}

void* axFontProvider_FreeType_MemRealloc(FT_Memory memory, long  cur_size, long new_size, void* block) {
	if (new_size <=0 ) {
		AX_ASSERT(false);
		return nullptr;
	}
	return ::realloc(block, (size_t)new_size);
}

void axFontProvider_FreeType_MemFree(FT_Memory memory, void* block) {
	::free(block);
}

axFontProvider_FreeType::axFontProvider_FreeType() {
	_allocatorWrapper.user		= ax_scope_allocator();
	_allocatorWrapper.alloc		= &axFontProvider_FreeType_MemAlloc;
	_allocatorWrapper.realloc	= &axFontProvider_FreeType_MemRealloc;
	_allocatorWrapper.free		= &axFontProvider_FreeType_MemFree;

	auto error = FT_New_Library(&_allocatorWrapper, &_lib);
	if ( error ) {
		throw axError_Undefined(AX_LOC);
	}		

	FT_Add_Default_Modules(_lib);
}

axFontProvider_FreeType::~axFontProvider_FreeType() {
	for (auto& f : _fonts) {
		if (f->refCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fonts.clear();
//-----
	for (auto& f : _fontFaces) {
		if (f->refCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fontFaces.clear();

//-----
	for (auto& f : _fontFiles) {
		if (f->refCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fontFiles.clear();

//-----
	if (_lib) {
		FT_Done_Library(_lib);
		_lib = nullptr;
	}
}

axFontFace* axFontProvider_FreeType::loadFontFile(axStrView filename, axStrView userDefineName) {
	auto o = axSPtr_new(axFontFile_FreeType(this));
	return o->loadFile(filename, userDefineName);
}

axFontFace* axFontProvider_FreeType::loadFontMem(axByteSpan mem, bool copyMem, axStrView userDefineName) {
	auto o = axSPtr_new(axFontFile_FreeType(this));
	return o->loadMem(mem, copyMem, userDefineName);
}

axFontFace* axFontProvider_FreeType::loadFontMem(axIByteArray && mem, axStrView userDefineName) {
	auto o = axSPtr_new(axFontFile_FreeType(this));
	return o->loadMem(ax_move(mem), userDefineName);
}

axSPtr<axFont> axFontProvider_FreeType::onCreateFont(const axFontDesc& desc) {
	for (auto& f : _fontFaces) {
		if (f->name() == desc.fontName) {
			return f->createFont(desc);
		}
	}
	throw axError_Undefined(AX_LOC);
}

void axFontProvider_FreeType::onNewFile(FontFile* f) {
	_fontFiles.append(axSPtr_make(f));
}

void axFontProvider_FreeType::onNewFont(Font* f) {
	_fonts.append(axSPtr_make(f));
}

void axFontProvider_FreeType::onNewFace(FontFaceFT* f) {
	_fontFaces.append(axSPtr_make(f));
}
