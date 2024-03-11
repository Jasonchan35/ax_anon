#pragma once

#include "axBBox2.h"

// axis-aligned bounding box (AABB)
template<class T>
class axBBox3 {
public:
	using Element = T;

	using Vec3   = axVec3<T>;
	using Tuple3 = axTuple3<T>;
	using Rect2	 = axRect2<T>;

	Vec3	min { kScalarMax(), kScalarMax(), kScalarMax() };
	Vec3	max { kScalarMin(), kScalarMin(), kScalarMin() };

	AX_STRUCT_BEGIN(axBBox3<T>, axNoBase)
		ax_FIELDS(min, max)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	axBBox3() {}
	axBBox3(const Vec3& min_, const Vec3& max_) : min(min_), max(max_) {}

	constexpr static axBBox3	kZero() { return axBBox3(Vec3(0), Vec3(0)); }

	AX_INLINE void clear() { *this = This(); };

	T		xMin() const { return min.x; }
	T		yMin() const { return min.y; }
	T		zMin() const { return min.z; }

	T		xMax() const { return max.x; }
	T		yMax() const { return max.y; }
	T		zMax() const { return max.z; }

	T		xSize() const { return isValid() ? max.x - min.x : 0; }
	T		ySize() const { return isValid() ? max.y - min.y : 0; }
	T		zSize() const { return isValid() ? max.z - min.z : 0; }

	Vec3	size() const { return isValid() ? Vec3(max.x - min.x, max.y - min.y, max.z - min.z) : Vec3(0,0,0); }

	void	encapsulate(const Vec3&   pt);
	void	encapsulate(const Tuple3& pt) { encapsulate(Vec3::s_cast(pt)); }
	void	encapsulate(const axBBox3& r);

	Rect2	toRect2() const;;

	bool	isValid() const { return min.x <= max.x; }
	explicit operator bool() const { return isValid(); }

	AX_INLINE static T	kScalarMin() { return axBBox1<T>::kScalarMin(); }
	AX_INLINE static T	kScalarMax() { return axBBox1<T>::kScalarMax(); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axBBox3<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

using axBBox3f = axBBox3<axFloat32>;
using axBBox3d = axBBox3<axFloat64>;

//---------------------
template<class T> inline
void axBBox3<T>::encapsulate(const Vec3& pt) {
	ax_min_it(min, pt);
	ax_max_it(max, pt);
}

template<class T> inline
void axBBox3<T>::encapsulate(const axBBox3& r) {
	ax_min_it(min, r.min);
	ax_max_it(max, r.max);
}

template<class T> inline
axRect2<T> axBBox3<T>::toRect2() const {
	if (!isValid()) return Rect2(0, 0, 0, 0);
	return Rect2(	min.x,
					min.y,
					max.x - min.x,
					max.y - min.y);
}
