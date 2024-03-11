#include "axRay3.h"
#include "axMathGeometry.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axRay3<T>)

template<class T>
axRay3<T> axRay3<T>::s_unProjectFromInvMatrix(const Vec2& screenPos, const Mat4& mat, const Rect2& viewport) {
	auto pt = screenPos;

	Vec3 p0(pt, 0);
	Vec3 p1(pt, 1);

	auto v0 = Vec3::s_unprojectInvMatrix(p0, mat, viewport);
	auto v1 = Vec3::s_unprojectInvMatrix(p1, mat, viewport);

	axRay3 o;
	o.origin = v0;
	o.dir = (v1 - v0).normal();
	return o;
}

template<class T>
bool axRay3<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto v = inPoint - origin;
	auto t = v.dot(dir);
	outPoint = (t <= 0) ? origin : origin + dir * t;
	return true;
}

template<class T>
bool axRay3<T>::getClosestPoint(Vec3& outPoint, const axLine3<T>& line, T minOnLine, T maxOnLine) const {
	auto da = line.end - line.start;
	auto db = dir;
	auto dc = origin - line.start;

	auto ab = da.cross(db);
	if (ab.equals0())
		return false;

	auto s = dc.cross(db).dot( da.cross(db) ) / ab.lengthSq();
	s = ax_clamp<T>(s, minOnLine, maxOnLine);
	outPoint = line.start + da * s;
	return true;
}

#define	AX_MACRO_OP(T) \
	template class axRay3<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
