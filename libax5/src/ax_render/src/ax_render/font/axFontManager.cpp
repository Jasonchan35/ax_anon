#include "axFontManager.h"
#include "FreeType/axFontProvider_FreeType.h"

axFontManager* axFontManager_instance;

axFontManager* axFontManager::s_instance() {
	return axFontManager_instance;
}

axFontManager::axFontManager() {
	AX_ASSERT(axFontManager_instance == nullptr);

	axFontManager_instance = this;

	_glyphCache.create(1024, 1024);

	_provider = axSPtr_new(axFontProvider_FreeType);
}

axFontManager::~axFontManager() {
}

axFontFace* axFontManager::loadFontFile(axStrView filename, axStrView userDefineName) {
	return _provider->loadFontFile(filename, userDefineName);
}

axSPtr<axFont> axFontManager::createFont(const FontDesc& desc) {
	return _provider->createFont(desc);
}

axSPtr<axFont> axFontManager::createFont(axStrView face, axInt size, bool bold, bool italic) {
	FontDesc desc;
	desc.fontName = face;
	desc.size = size;
	desc.bold = bold;
	desc.italic = italic;
	return createFont(desc);
}

void axFontManager::onUpdateGpuResources(axRenderCommandDispatcher& dispatcher) {
	_glyphCache.onUpdateGpuResources(dispatcher);
}

void axFontManager::onDebugRender(axRenderRequest& req) {
	_glyphCache.onDebugRender(req);
}
