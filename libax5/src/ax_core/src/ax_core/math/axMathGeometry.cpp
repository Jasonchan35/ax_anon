#include "axMathGeometry.h"
#include "axRay3.h"
#include "axBBox2.h"
#include "axBBox3.h"

template<class T>
void axSphere3<T>::onFormat(axFormat& f) const {
	f << "Sphere[center=" << center << ", radius=" << radius << "]";
}

template<class T>
void axPlane3<T>::onFormat(axFormat& f) const {
	f << "Plane[normal=" << normal << ", distance=" << distance << "]";
}

template<class T>
bool axLine2<T>::getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const {

	axLine3<T> line3(start.xy0(), end.xy0());
	axVec3<T> closestPt;
	if (!line3.getClosestPoint(closestPt, inPoint.xy0()))
		return false;

	outPoint = closestPt.xy();
	return true;
}

template<class T>
T axLine2<T>::distanceToPoint(const Vec2& pt) const {
	Vec2 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return axNumLimit<T>::inf();
}

template<class T>
bool axLine3<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto line = end - start;
	auto len = line.length();

	if (ax_math_equals0(len))
		return false;

	auto u = line / len; //normalized line
	auto v = inPoint - start;

	auto w = u.dot(v);
	w = ax_clamp(w, T(0), len);

	outPoint = start + u * w;
	return true;
}

template<class T>
bool axLine3<T>::getClosestPoint(Vec3& outPoint, const Ray3& ray) const {
	auto da = end - start;
	auto db = ray.dir;
	auto dc = ray.origin - start;

	auto ab = da.cross(db);
	if (ab.equals0())
		return false;

	auto s = dc.cross(db).dot( da.cross(db) ) / ab.lengthSq();
	s = ax_clamp01(s);
	outPoint = start + da * s;
	return true;
}

template<class T>
T axLine3<T>::distanceToPoint(const Vec3& pt) const {
	Vec3 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return axNumLimit<T>::inf();
}


#define	AX_MACRO_OP(T) \
	template class axLine2<T>; \
	template class axLine3<T>; \
	template class axPlane3<T>; \
	template class axSphere3<T>; \
	template class axCylinder3<T>; \
	template class axCapsule3<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
