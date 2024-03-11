#pragma once

#include <ax_ui/text/axUITextStyle.h>

class axUIApp;

class axUIDefaultAssets : public axNonCopyable {
public:
	static axUIDefaultAssets* s_instance();

	struct FontStyles {
		axSPtr<axFontStyle>	std;
		axSPtr<axFontStyle>	stdWithBg;
	};
	FontStyles fontStyles;

	struct Textures {
		axSPtr<axTexture2D>	diaSocketCircle;
	};

	Textures	textures;

	axUIDefaultAssets();
	~axUIDefaultAssets();

	void load(axUIApp* app, axStrView assetsDir);

	axAllocator* allocator() { return ax_default_allocator(); }
};
