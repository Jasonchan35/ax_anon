#pragma once

class axUIFillStyle {
public:
	using Sprite = axTextureSprite;
	using Color = axColor;
	using Vec2  = axVec2f;
	using Rect2 = axRect2f;

	Sprite	sprite;
	Color	color;

	void draw(const axSourceLoc& loc, axRenderRequest& req, const Rect2& rect);
};