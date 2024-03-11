#include "GEO_Transform.h"

namespace Anon {

void GEO_Transform::onCompute() {
	Base::onCompute();
	if (bypass()) return;

	auto mat = Mat4d::s_TRS_Deg(_translate, _rotate, _scale);

	auto srcSpan = input().readPoints();
	auto dstSpan = _output.editPoints();

	auto dst = ax_foreach_begin(dstSpan);
	for (auto& src : srcSpan) {
		dst->pos = mat.mulPoint(src.pos);
		dst++;
	}
}

} //namespace