#pragma once
#include "axImage.h"

template<class COLOR>
class axImagePainter_  : public axNonCopyable {
public:
	using Vec2 = axVec2f;
	using Rect2 = axRect2f;
	using Margin2 = axMargin2f;
	using Color = COLOR;

	axImagePainter_(axImage& img) : _img(img) { _checkType(); }

	void fill(const Color& color) { _img.fill(color); }

	void drawAACircle				(const Vec2& center, float radius, const Color& color);
	void drawAACircleOutline		(const Vec2& center, float radius, const Color& color, float outlineWidth);

	void drawAARoundedRect			(const Rect2& rect, float radius, const Color& color);
	void drawAARoundedRectOutline	(const Rect2& rect, float radius, const Color& color, float outlineWidth);

private:
	void _checkType() {
		if (_img.colorType() != Color::kColorType)
			throw axError_Undefined(AX_LOC);
	}

	template<bool OUTLINE> void _drawAACircleImpl(const Vec2& center, float radius, const Color& color, float outlineWidth);
	template<bool OUTLINE> void _drawAARoundedRectImpl(const Rect2& rect, float radius, const Color& color, float outlineWidth);

	AX_INLINE axMutSpan<Color> _row(axInt y) { return _img.template row_noCheck<Color>(y); }

	axImage& _img;
};

template<class COLOR> inline
axImagePainter_<COLOR> axImage::painter() {
	return axImagePainter_<COLOR>(*this);
}

