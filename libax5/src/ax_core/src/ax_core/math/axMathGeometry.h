#pragma once

#include "axQuat4.h"
#include <ax_core/other/axEnum.h>

#define axAxisDir_EnumList(E) \
	E(X,) \
	E(Y,) \
	E(Z,) \
//---

ax_ENUM_CLASS(axAxisDir, axUInt8)

template<class T>
class axRay3;

template<class T>
class axSphere3 {
	using Vec3 = axVec3<T>;
public:
	axSphere3() = default;
	axSphere3(const Vec3& center_, T radius_) : center(center_), radius(radius_) {}
	void onFormat(axFormat& f) const;

	Vec3	center;
	T		radius;
};

template<class T>
class axTriangle3 {
	using Vec3 = axVec3<T>;
public:
	axTriangle3() = default;
	axTriangle3(const Vec3& v0_, const Vec3& v1_, const Vec3& v2_) : v0(v0_), v1(v1_), v2(v2_) {}

	Vec3 v0, v1, v2;
};

template<class T>
class axPlane3 {
	using Vec3 = axVec3<T>;
public:
	axPlane3() = default;
	axPlane3(const Vec3& normal_, T distance_)
		: normal(normal_)
		, distance(distance_) 
	{}

	axPlane3(const Vec3& normal_, const Vec3& point_)
		: normal(normal_)
		, distance(normal_.dot(point_))
	{}

	axPlane3(const axTriangle3<T>&  tri) {
		normal = (tri.v1 - tri.v0).cross(tri.v2 - tri.v0).normal();
		distance = normal.dot(tri.v0);
	}

	void onFormat(axFormat& f) const;

	Vec3	normal;
	T		distance; // distance from origin
};

template<class T>
class axCylinder3 {
	using Vec3 = axVec3<T>;
public:
	axCylinder3() = default;
	axCylinder3(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	Vec3	start;
	Vec3	end;
	T		radius;
};

template<class T>
class axCapsule3 {
	using Vec3 = axVec3<T>;
public:
	axCapsule3() = default;
	axCapsule3(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	Vec3	start;
	Vec3	end;
	T		radius;
};


template<class T>
class axLine2 {
	using Vec2 = axVec2<T>;
public:
	axLine2() = default;
	axLine2(const Vec2& start_, const Vec2& end_)
		: start(start_)
		, end(end_)
	{}

	bool getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const;
	T	distanceToPoint(const Vec2& pt) const;

	Vec2	start;
	Vec2	end;
};

template<class T>
class axLine3 {
	using Vec3 = axVec3<T>;
	using Ray3 = axRay3<T>;
public:
	axLine3() = default;
	axLine3(const Vec3& start_, const Vec3& end_)
		: start(start_)
		, end(end_)
	{}

	bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	bool getClosestPoint(Vec3& outPoint, const Ray3& ray    ) const;

	T	distanceToPoint(const Vec3& pt) const;

	Vec3	start;
	Vec3	end;
};

//-----------------------------------------
using axSphere3f = axSphere3<axFloat32>;
using axSphere3d = axSphere3<axFloat64>;

using axPlane3f  = axPlane3<axFloat32>;
using axPlane3d  = axPlane3<axFloat64>;

using axTriangle3f = axTriangle3<axFloat32>;
using axTriangle3d = axTriangle3<axFloat64>;

using axCapsule3f = axCapsule3<axFloat32>;
using axCapsule3d = axCapsule3<axFloat64>;

using axLine2f = axLine2<axFloat32>;
using axLine2d = axLine2<axFloat64>;

using axLine3f = axLine3<axFloat32>;
using axLine3d = axLine3<axFloat64>;
