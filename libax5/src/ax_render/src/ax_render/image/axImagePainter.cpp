#include "axImagePainter.h"

class axImagePainter_RoundedCornerWeight {
public:
	axImagePainter_RoundedCornerWeight(float r_) : r(r_), r2(r * r) {}
	AX_INLINE float _t(float x, float y) {
		if (x == 0) return r - y;
		return ax_sqrt(ax_abs(r2 - x * x)) - y;
	}

	AX_INLINE float weight(const axVec2f& d) {
		float t = (d.x < d.y) 
				? _t(d.x, d.y)
				: _t(d.y, d.x);

		float o = ax_clamp01(t + 0.25f);
		return o;
	}

	float r;
	float r2;
};

template<class COLOR>
template<bool OUTLINE> inline
void axImagePainter_<COLOR>::_drawAACircleImpl(const Vec2& center, float radius, const Color& color, float outlineWidth) {
	_checkType();

	AX_IF_CONSTEXPR(OUTLINE) {
		if (outlineWidth <= 0) return;
	}

	float innerRadius = radius - outlineWidth;
	if (innerRadius <= 0) {
		return drawAACircle(center, radius, color);
	}

	axInt w = _img.width();
	axInt h = _img.height();
	if (w <= 0 || h <= 0) return;

	axInt sx = ax_clamp(ax_floor_to_int(center.x - radius), axInt(0), w);
	axInt sy = ax_clamp(ax_floor_to_int(center.y - radius), axInt(0), h);

	axInt ex = ax_clamp(ax_ceil_to_int( center.x + radius), axInt(0), w);
	axInt ey = ax_clamp(ax_ceil_to_int( center.y + radius), axInt(0), h);

	axImagePainter_RoundedCornerWeight outer(radius);
	axImagePainter_RoundedCornerWeight inner(innerRadius);

	for (axInt y = sy; y < ey; y++) {
		auto row = _row(y);
		auto pixel = ax_foreach_begin(row.slice(sx, ex - sx));

		for (axInt x = sx; x < ex; x++, pixel++) {
			auto pos = Vec2::s_cast(axVec2i(x, y)) + 0.5f;
			auto d = ax_abs(pos - center);

			float weight = outer.weight(d);

			AX_IF_CONSTEXPR(OUTLINE) {
				float innerWeight = inner.weight(d);
				weight = ax_clamp01(weight - innerWeight);
			}

			if (weight == 0) continue;

			pixel->blend(color, weight);
		}
	}
}

template<class COLOR>
void axImagePainter_<COLOR>::drawAACircle(const Vec2& center, float radius, const Color& color) {
	_drawAACircleImpl<false>(center, radius, color, 0);
}

template<class COLOR>
void axImagePainter_<COLOR>::drawAACircleOutline(const Vec2& center, float radius, const Color& color, float outlineWidth) {
	_drawAACircleImpl<true>(center, radius, color, outlineWidth);
}

template<class COLOR>
template<bool OUTLINE>
void axImagePainter_<COLOR>::_drawAARoundedRectImpl(const Rect2& rect, float radius, const Color& color, float outlineWidth) {
	_checkType();

	AX_IF_CONSTEXPR(OUTLINE) {
		if (outlineWidth <= 0) return;
	}

	axInt w = _img.width();
	axInt h = _img.height();
	if (w <= 0 || h <= 0) return;

	axInt sx = ax_clamp(ax_floor_to_int(rect.xMin()), axInt(0), w);
	axInt sy = ax_clamp(ax_floor_to_int(rect.yMin()), axInt(0), h);

	axInt ex = ax_clamp(ax_ceil_to_int(rect.xMax()), axInt(0), w);
	axInt ey = ax_clamp(ax_ceil_to_int(rect.yMax()), axInt(0), h);

	class Helper : public axNonCopyable {
	public:
		Helper(float radius, const Rect2& outerRect, const Rect2& innerRect)
			: corner(radius)
			, outerBox(outerRect)
			, innerBox(innerRect)
		{}

		AX_INLINE
		float weight(const Vec2& pos) {
			if (pos.x >= innerBox.min.x && pos.x <= innerBox.max.x) {
				if (pos.y < innerBox.min.y) {
					return 1 - ax_clamp01(outerBox.min.y - pos.y + 0.5f); // top
				} else if (pos.y > innerBox.max.y) {
					return 1 - ax_clamp01(pos.y - outerBox.max.y + 0.5f); // bottom
				} else {
					return 1;
				}
			}
			
			if (pos.y >= innerBox.min.y && pos.y <= innerBox.max.y) {
				if (pos.x < innerBox.min.x) {
					return 1 - ax_clamp01(outerBox.min.x - pos.x + 0.5f); // left
				} else if (pos.x > innerBox.max.x) {
					return 1 - ax_clamp01(pos.x - outerBox.max.x + 0.5f); // right
				} else {
					return 1;
				}
			}

			if (pos.y < innerBox.min.y) {
				if (pos.x <= innerBox.min.x) {
					return corner.weight(Vec2(innerBox.min.x - pos.x, innerBox.min.y - pos.y)); // top left
				} else {
					return corner.weight(Vec2(pos.x - innerBox.max.x, innerBox.min.y - pos.y)); // top right
				}
			} else {
				if (pos.x <= innerBox.min.x) {
					return corner.weight(Vec2(innerBox.min.x - pos.x, pos.y - innerBox.max.y)); // bottom left
				} else {
					return corner.weight(Vec2(pos.x - innerBox.max.x, pos.y - innerBox.max.y)); // bottom right
				}
			}
		}

		axImagePainter_RoundedCornerWeight corner;
		axBBox2f outerBox;
		axBBox2f innerBox;
	};

	auto& outerRect = rect;
	auto  innerRect = rect - Margin2(radius);

	Helper helper(radius, outerRect, innerRect);
	Helper outlineHelper(	radius - outlineWidth + 0.25f,
							outerRect - Margin2(outlineWidth),
							innerRect);

	for (axInt y = sy; y < ey; y++) {
		auto row = _row(y);
		auto* pixel = &row[sx];

		for (axInt x = sx; x < ex; x++, pixel++) {
			auto pos = Vec2::s_cast(axVec2i(x, y)) + 0.5f;

			float weight = helper.weight(pos);

			AX_IF_CONSTEXPR(OUTLINE) {
				float innerWeight = outlineHelper.weight(pos);
				weight = ax_clamp01(weight - innerWeight);
			}

			if (weight == 0) continue;

			pixel->blend(color, weight);
		}
		AX_ASSERT(pixel == row.data() + ex);
	}
}

template<class COLOR>
void axImagePainter_<COLOR>::drawAARoundedRect(const Rect2& rect, float radius, const Color& color) {
	_drawAARoundedRectImpl<false>(rect, radius, color, 0);
}

template<class COLOR>
void axImagePainter_<COLOR>::drawAARoundedRectOutline(const Rect2& rect, float radius, const Color& color, float outlineWidth) {
	_drawAARoundedRectImpl<true>(rect, radius, color, outlineWidth);
}

#define	AX_MACRO_OP(COLOR)	\
	template class axImagePainter_<COLOR>; \
//-------------
	AX_MACRO_OP(axColorRGBAb)
	AX_MACRO_OP(axColorRGBAf)
#undef AX_MACRO_OP
