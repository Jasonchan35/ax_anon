#include "axRayCast3.h"

template<class T>
bool axRayCast3<T>::hitTest(const Ray3& ray, const Plane3& plane, bool testBothSize) {
	auto s = ray.dir.dot(plane.normal);

	if (ax_math_equals0(s))
		return false;

	bool backFace = s > 0;
	if (!testBothSize && backFace)
		return false;

	auto nl = backFace ? -plane.normal : plane.normal;

	auto dis = (plane.distance - ray.origin.dot(nl)) / s;
	if (dis < 0 || dis > result.distance)
		return false;

	result.distance = dis;
	result.point  = ray.origin + (ray.dir * dis);
	result.normal = nl;
	result.objectType = ObjectType::Plane3;
	return true;
}

template<class T>
bool axRayCast3<T>::hitTest(const Ray3& ray, const Triangle3& tri, bool testBothSize) {
	Plane3 plane(tri);

	axRayCast3 rc;
	rc.result.distance = result.distance;
	if (!rc.hitTest(ray, plane, testBothSize))
		return false;

	//edge
	auto e0 = tri.v1 - tri.v0;
	auto e1 = tri.v2 - tri.v1;
	auto e2 = tri.v0 - tri.v2;

	auto t0 = rc.result.point - tri.v0;
	auto t1 = rc.result.point - tri.v1;
	auto t2 = rc.result.point - tri.v2;

	if (e0.cross(t0).dot(plane.normal) < 0) return false;
	if (e1.cross(t1).dot(plane.normal) < 0) return false;
	if (e2.cross(t2).dot(plane.normal) < 0) return false;

	result = rc.result;
	result.objectType = ObjectType::Triangle3;
	return true;
}

template<class T>
bool axRayCast3<T>::hitTest(const Ray3& ray, const Sphere3& sphere) {
	auto m = ray.origin - sphere.center;
	auto b = m.dot(ray.dir);
	auto c = m.dot(m) - sphere.radius * sphere.radius;

	if (c > 0 && b > 0)
		return false;

	auto discriminant  = b * b - c;

	if (discriminant < 0)
		return false;

	auto t = - b - ax_sqrt(discriminant);
	if (t < 0 || t > result.distance)
		return false;

	result.distance = t;
	result.point = ray.origin + ray.dir * t;
	result.normal = (result.point - sphere.center).normal();
	result.objectType = ObjectType::Sphere3;
	return true;
}

template<class T>
bool axRayCast3<T>::hitTest(const Ray3& ray, const Cylinder3& cylinder) {
	auto ab = cylinder.end - cylinder.start;
	auto ao = ray.origin - cylinder.start;
	auto r  = cylinder.radius;

	auto ao_ab  = ao.cross(ab);
	auto dir_ab = ray.dir.cross(ab);
	auto ab2 = ab.dot(ab);

	//Quadratic equation
	auto A = dir_ab.dot(dir_ab);
	auto B = 2 * dir_ab.dot(ao_ab);
	auto C = ao_ab.dot(ao_ab) - r * r * ab2;

	auto s = ax_sqrt(B * B - 4 * A * C);

	auto t0 = (-B + s) / (2 * A);
	auto t1 = (-B - s) / (2 * A);

	auto t = ax_min(t0, t1);
	if (t < 0 || t > result.distance)
		return false;

	auto hitPt = ray.origin + ray.dir * t;

	//find center point along the line
	auto v = hitPt - cylinder.start;
	auto abLen = ab.length();
	auto u = ab / abLen;

	auto d = v.dot(u);
	if (d < 0 || d > abLen) //center point out of range
		return false;

	auto cp = cylinder.start + u * d;

	result.distance = t;
	result.point = hitPt;
	result.normal = (hitPt - cp).normal();
	result.objectType = ObjectType::Cylinder3;
	return true;
}

template<class T>
bool axRayCast3<T>::hitTest(const Ray3& ray, const Capsule3& capsule) {
	auto p0 = capsule.start;
	auto p1 = capsule.end;
	auto r  = capsule.radius;

	auto v = (p1 - p0).normal();
	if (v.cross(ray.dir).equals0()) {
		//parallel to capsule direction, do test on sphere end
		Sphere3 sphere(v.dot(ray.dir) > 0 ? p0 : p1, capsule.radius);
		return hitTest(ray, sphere);
	}

	Vec3 pointOnLine;
	Line3 line(p0, p1);
	if (!line.getClosestPoint(pointOnLine, ray))
		return false;

	Vec3 pointOnRay;
	ray.getClosestPoint(pointOnRay, pointOnLine);
	auto dis = pointOnRay.distance(pointOnLine);

	if (dis > r)
		return false;

	bool hit = false;
	Cylinder3 cylinder(p0, p1, r);
	if (hitTest(ray, cylinder)) {
		hit = true;
	}

	Sphere3 sphere0(capsule.start, capsule.radius);
	if (hitTest(ray, sphere0)) {
		hit = true;
	}

	Sphere3 sphere1(capsule.end, capsule.radius);
	if (hitTest(ray, sphere1)) {
		hit = true;
	}

	result.objectType = ObjectType::Capsule3;
	return hit;
}

#define	AX_MACRO_OP(T) \
	template class axRayCast3<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
