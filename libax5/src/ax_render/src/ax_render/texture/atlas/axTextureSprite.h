#pragma once

#include "axTextureAtlas.h"
#include "../axTexture.h"

class axTextureSprite : public axObject {
public:
	using Atlas		= axTextureAtlas;
	using Tex2D		= axTexture2D;
	using Vec2		= axVec2f;
	using Rect2		= axRect2f;
	using Margin2	= axMargin2f;

		  Atlas*	atlas	()			{ return _atlas.ptr(); }
		  Tex2D*	texture	()			{ return _texture.ptr(); }

			Vec2	size	() const	{ return _texture ? Vec2::s_cast(_texture->size()) : Vec2(0, 0); }
	const	Vec2&	pivot	() const	{ return _pivot; }

	const	Rect2&	rectInTexture	() const { return _rectInTexture; }
			Rect2	uvRectInTexture	() const { return _rectInTexture / size(); }

			Rect2	toTextureSpace	(const Rect2& uv) const { auto s = uvRectInTexture(); return (uv * s.size) + s.pos;  }
			Rect2	fromTextureSpace(const Rect2& uv) const { auto s = uvRectInTexture(); return (uv - s.pos ) / s.size; }

	const Margin2&	border		() const	{ return _border; }
			bool	hasCenter	() const	{ return _hasCenter; }

	void set(Atlas* atlas, Tex2D* texture, const Rect2& rect, const Margin2& border = {0}, const Vec2& pivot = {0, 0}, bool fillCenter = true);

private:
	axLink<Atlas>	_atlas;
	axSPtr<Tex2D>	_texture;
	Rect2			_rectInTexture{0,0,0,0};
	Margin2			_border{0};
	Vec2			_pivot;
	bool			_hasCenter = true;
};