#pragma once

#include "axMathGeometry.h"

template<class T>
class axRay3 {
	using Vec2		= axVec2<T>;
	using Vec3		= axVec3<T>;
	using Vec4		= axVec4<T>;
	using Mat4		= axMat4<T>;
	using Quat4		= axQuat4<T>;
	using Rect2		= axRect2<T>;
	using Line3		= axLine3<T>;
public:

	Vec3 origin, dir;

	AX_STRUCT_BEGIN(axRay3<T>, axNoBase)
		ax_FIELDS(origin, dir)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	axRay3() = default;
	axRay3(const Vec3& origin_, const Vec3& dir_) : origin(origin_), dir(dir_) {}

	constexpr static axRay3 s_zero() { return axRay3(Vec3(0), Vec3(0)); }

	static axRay3 s_unProjectFromInvMatrix(const Vec2& screenPos, const Mat4& invMat, const Rect2& viewport);

	struct HitTestResult {
		T		distance = ax_inf_<T>();
		Vec3	point;
		Vec3	normal;

		bool	hasResult() const { return distance != ax_inf_<T>(); }
	};

	bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	bool getClosestPoint(Vec3& outPoint, const axLine3<T>& line, T minOnLine = 0, T maxOnLine = 1) const;
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axRay3<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

using axRay3f = axRay3< axFloat32 >;
using axRay3d = axRay3< axFloat64 >;
