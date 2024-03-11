#include <ax_render/axRenderer.h>
#include <ax_ui/base/axUIApp.h>
#include <ax_ui/base/axUIView.h>

#include "axUIDefaultAssets.h"

axUIDefaultAssets* axUIDefaultAssets_instance;

axUIDefaultAssets* axUIDefaultAssets::s_instance() {
	return axUIDefaultAssets_instance;
}

axUIDefaultAssets::axUIDefaultAssets() {
	AX_ASSERT(axUIDefaultAssets_instance == nullptr);
	axUIDefaultAssets_instance = this;
}

axUIDefaultAssets::~axUIDefaultAssets() {
	AX_ASSERT(axUIDefaultAssets_instance == this);
	axUIDefaultAssets_instance = nullptr;

	auto* renderer = axRenderer::s_instance();
	renderer->setDefaultFontStyle(nullptr);
}

void axUIDefaultAssets::load(axUIApp* app, axStrView assetsDir) {

	auto* renderer = axRenderer::s_instance();
	axSamplerState samplerState;
	samplerState.filter = axTextureFilter::Point;

	{
		axImageRGBAb img;
		img.create(16, 16);
//		img.create(64, 64); DX12 RowPitch issue
		auto center = axVec2f(8);

		auto painter = img.painter();
		painter.fill(axColorRGBAb::kZero());
		painter.drawAACircle(center, 6, {255, 0, 0});
		painter.drawAACircleOutline(center, 6, {0, 255, 0}, 1.5f);

		textures.diaSocketCircle = renderer->createTexture2D(img, samplerState);
	}


	auto* fm = axFontManager::s_instance();

	fontStyles.std = axSPtr_new(axFontStyle);

	renderer->setDefaultFontStyle(fontStyles.std);

	axStrLiteral	fontFile;
	axInt			fontSize;

	switch (3) {
		case 0: {
			fontFile = "/fonts/Antic/Antic-Regular.ttf";
			fontSize = 20;
		}break;

		case 1: {
			fontFile = "/fonts/Roboto/Roboto-Regular.ttf";
			fontSize = 14;
		}break;

		case 2: {
			fontFile = "/fonts/NotoSerif/NotoSerif-Regular.ttf";
			fontSize = 14;
		}break;

		case 3: {
			fontFile = "/fonts/Monospace/Ubuntu_Mono/UbuntuMono-Bold.ttf";
			fontSize = 15;
		}break;

		case 4: {
			fontFile = "/fonts/Monospace/DM_Mono/DMMono-Medium.ttf";
			fontSize = 13;
		}break;

		default: {
			fontFile = "/fonts/Monospace/FiraCode/FiraCode-Medium.ttf";
			fontSize = 13;
		}break;
	}

	{
		axTempString fullpath;
		fullpath.set(assetsDir, fontFile);
		auto* fontFace = fm->loadFontFile(fullpath);
		fontStyles.std->font = fm->createFont(fontFace->name(), fontSize);
	}
	fontStyles.stdWithBg = axSPtr_new(axFontStyle(*fontStyles.std));
	fontStyles.stdWithBg->bgColor.set(.1f, .1f, .1f, 1);
}
