#include "axVec3.h"
#include "axVec4.h"
#include "axQuat4.h"
#include <ax_core/rtti/axTypeManager.h>

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axVec3<T>)

template<class T>
axVec3<T> axVec3<T>::s_unprojectInvMatrix(const Vec3& screenPos, const Mat4& invMat, const Rect2& viewport) {
	auto  tmp = Vec4(screenPos, 1);
	tmp.y = viewport.h - tmp.y; // y is down

	tmp.x = (tmp.x - viewport.x) / viewport.w * 2 - 1;
	tmp.y = (tmp.y - viewport.y) / viewport.h * 2 - 1;

	auto obj = invMat.mulPoint(tmp);
	return obj.homogenize();
}

#define	AX_MACRO_OP(T) \
	template class axVec3<T>; \
//-------------
	AX_MACRO_OP(axInt)
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
