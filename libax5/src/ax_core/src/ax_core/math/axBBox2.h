#pragma once

#include "axQuat4.h"
#include "axBBox1.h"

// axis-aligned bounding box (AABB)
template<class T>
class axBBox2 {
public:
	using Element = T;

	using Vec2   = axVec2<T>;
	using Tuple2 = axTuple2<T>;
	using Rect2	 = axRect2<T>;

	Vec2	min { kScalarMax(), kScalarMax() };
	Vec2	max { kScalarMin(), kScalarMin() };

	AX_STRUCT_BEGIN(axBBox2<T>, axNoBase)
		ax_FIELDS(min, max)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	axBBox2() {}
	axBBox2(const Vec2& min_, const Vec2& max_) : min(min_), max(max_) {}
	axBBox2(const Rect2& rect) : min(rect.xMinYMin()), max(rect.xMaxYMax()) {}

	AX_INLINE void clear() { *this = This(); };

	T		xMin() const { return min.x; }
	T		yMin() const { return min.y; }

	T		xMax() const { return max.x; }
	T		yMax() const { return max.y; }

	Vec2	size() const { return Vec2(xSize(), ySize()); }
	T		xSize() const { return isValid() ? max.x - min.x : 0; }
	T		ySize() const { return isValid() ? max.y - min.y : 0; }

	void	include(const Vec2&   pt);
	void	include(const Tuple2& pt)	{ include(Vec2::s_cast(pt)); }
	void	include(const axBBox2& r);
	void	include(const Rect2&  r)	{ include(r.xMinYMin()); include(r.xMaxYMax()); }

	bool	pointIsInside(const Vec2& pt) const {
		return pt.x >= min.x && pt.y >= min.y 
			&& pt.x <= max.x && pt.y <= max.y;
	}

	Rect2	toRect2() const;

	bool	isValid() const { return min.x <= max.x; }
	explicit operator bool() const { return isValid(); }

	AX_INLINE static T	kScalarMin() { return axBBox1<T>::kScalarMin(); }
	AX_INLINE static T	kScalarMax() { return axBBox1<T>::kScalarMax(); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axBBox2<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

using axBBox2f = axBBox2<axFloat32>;
using axBBox2d = axBBox2<axFloat64>;

// ---- inline -----------
template<class T> inline
void axBBox2<T>::include(const Vec2& pt) {
	ax_min_it(min, pt);
	ax_max_it(max, pt);
}

template<class T> inline
void axBBox2<T>::include(const axBBox2& r) {
	ax_min_it(min, r.min);
	ax_max_it(max, r.max);
}

template<class T> inline
axRect2<T> axBBox2<T>::toRect2() const {
	if (!isValid()) return Rect2(0, 0, 0, 0);
	return Rect2(	min.x,
					min.y,
					max.x - min.x,
					max.y - min.y);
}
