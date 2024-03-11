#pragma once

#include "axRay3.h"

template<class T>
class axRayCast3 {
	using Vec2		= axVec2<T>;
	using Vec3		= axVec3<T>;
	using Vec4		= axVec4<T>;
	using Mat4		= axMat4<T>;
	using Ray3		= axRay3<T>;

	using Line3		= axLine3<T>;
	using Plane3	= axPlane3<T>;
	using Triangle3 = axTriangle3<T>;
	using Sphere3	= axSphere3<T>;
	using Cylinder3 = axCylinder3<T>;
	using Capsule3	= axCapsule3<T>;
public:

	enum class ObjectType {
		None,
		Plane3,
		Triangle3,
		Sphere3,
		Cylinder3,
		Capsule3,
	};

	struct Result {
		T			distance = ax_inf_<T>();
		Vec3		point;
		Vec3		normal;
//		Vec3		barycentric; // barycentric coordinate of the triangle
//		axInt		meshTriangleIndex = 0;
		ObjectType	objectType = ObjectType::None;

		explicit operator bool() const { return objectType != ObjectType::None; }
	};

	Result result;

	bool hitTest(const Ray3& ray, const Plane3&    plane, bool testBothSize);
	bool hitTest(const Ray3& ray, const Triangle3& tri,   bool testBothSize);
	bool hitTest(const Ray3& ray, const Sphere3&   sphere);
	bool hitTest(const Ray3& ray, const Cylinder3& cylinder);
	bool hitTest(const Ray3& ray, const Capsule3&  capsule);
};

using axRayCast3f = axRayCast3< axFloat32 >;
using axRayCast3d = axRayCast3< axFloat64 >;
