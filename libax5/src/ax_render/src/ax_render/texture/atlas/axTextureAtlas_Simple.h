#pragma once

#include "axTextureAtlas.h"

class axTextureAtlas_Simple : public axTextureAtlas {
public:
	using Tex2D		= axTexture2D;
	using Sprite	= axTextureSprite;

	Tex2D* texture() { return _texture; }


private:
	axSPtr<Tex2D>			_texture;
	axArray<axSPtr<Sprite>>	_sprites;
};
