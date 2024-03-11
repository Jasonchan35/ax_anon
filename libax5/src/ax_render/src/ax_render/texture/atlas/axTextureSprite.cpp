#include "axTextureSprite.h"

void axTextureSprite::set(Atlas* atlas, Tex2D* texture, const Rect2& rectInTexture, const Margin2& border, const Vec2& pivot, bool hasCenter) {
	_atlas.ref(atlas);
	_texture.ref(texture);
	_rectInTexture	= rectInTexture;
	_border			= border;
	_pivot			= pivot;
	_hasCenter		= hasCenter;
}
