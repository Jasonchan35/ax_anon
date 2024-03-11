#include "GEO_Bend.h"

namespace Anon {

GEO_Bend::GEO_Bend(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}

void GEO_Bend::onCompute() {
	Base::onCompute();
	if (bypass()) return;

	if (ax_math_equals0(_angle)) return;
	ax_max_it(_length, 0.0);

	auto srcPointNormals = input().readPointNormals();
	auto hasNormals = srcPointNormals.size() > 0;

	auto srcSpan = input().readPoints();
	auto dstSpan = _output.editPoints();

	auto dstPointNormals = hasNormals ? _output.editPointNormals() : axMutSpan<Vec3d>();

	auto dir = _direction.normal();
	auto up  = _upVector.normal();
	auto rotateAxis = dir.cross(up);

	auto angleInRad = ax_radians(_angle);

	auto radius = _length / angleInRad;
	auto pivot  = _origin - up * radius;

	auto dst = ax_foreach_begin(dstSpan);	
	for (axInt i = 0; i < srcSpan.size(); i++, dst++) {
		auto& src = srcSpan[i];
		auto t = (src.pos - _origin).dot(dir);
		if (t <= 0) {
			dst->pos = src.pos;
			if (hasNormals) {
				dstPointNormals[i] = srcPointNormals[i];
			}

		} else {
			auto weight = _length <= 0 ? 1 : t / _length;
			ax_clamp01_it(weight);

			auto projectOnDir = dir * weight * _length;
			auto pt = src.pos - projectOnDir - pivot;
			
			auto quat = Quat4d::s_angleAxis(angleInRad * weight, rotateAxis);
			dst->pos = pt * quat + pivot;

			if (hasNormals) {
				dstPointNormals[i] = srcPointNormals[i] * quat;
			}
		}
	}
}


} //namespace