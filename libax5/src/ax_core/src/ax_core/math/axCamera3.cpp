#include "axCamera3.h"

template<class T>
void axCamera3<T>::pan(T x, T y) {
	auto v = _aim - _pos;
	auto right = _up.cross(v.normal());

	auto q = Quat4::s_eulerY(x) * Quat4::s_angleAxis(y, right);
	v   *= q;
	_up *= q;
	_aim = _pos + v;
}

template<class T>
void axCamera3<T>::orbit(T x, T y) {
	auto v = _pos - _aim;
	auto right = _up.cross(v.normal());

	auto q = Quat4::s_eulerY(x) * Quat4::s_angleAxis(y, right);
	v   *= q;
	_up *= q;
	_pos = _aim + v;
}

template<class T>
void axCamera3<T>::move(T x, T y, T z) {
	auto v = _aim - _pos;
	auto dir = v.normal();
	auto right = _up.cross(dir);

	auto t = right * x + _up * y + dir * z;
	_pos += t;
	_aim += t;
}

template<class T>
void axCamera3<T>::dolly(T z) {
	auto v = _pos - _aim;
	auto dir = v.normal();
	auto d = v.length();
	d += z;
	ax_max_it(d, static_cast<T>(0.001));
	
	_pos = _aim + dir * d;
}

template<class T>
axRay3<T> axCamera3<T>::getRay(const Vec2& screenPos) const {
	return Ray3::s_unProjectFromInvMatrix(screenPos, viewProjMatrix().inverse(), _viewport);
}

template<class T>
axMat4<T> axCamera3<T>::viewMatrix() const {
	return Mat4::s_lookAt(_pos, _aim, _up);
}

template<class T>
axMat4<T> axCamera3<T>::projMatrix() const {
	T aspect = _viewport.h != 0 ? _viewport.w / _viewport.h : T(0);
	return Mat4::s_perspective(ax_radians(_fov), aspect, _nearClip, _farClip);
}

#define	AX_MACRO_OP(T) \
	template class axCamera3<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
