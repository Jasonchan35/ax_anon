#include "axMat4.h"
#include "axQuat4.h"
#include <ax_core/rtti/axTypeManager.h>

template<class T> void axMat4<T>::onJsonReader(axJsonReader& rd)		{ rd.reflection(*this); }
template<class T> void axMat4<T>::onJsonWriter(axJsonWriter& wr) const	{ wr.reflection(*this); }

template<class T>
void axMat4<T>::onFormat(axFormat& f) const {
	f << "[\n" << cx << ",\n" << cy << ",\n" << cz << ",\n" << cw << "\n]";
}

template<class T>
axMat4<T> axMat4<T>::inverse() const {
#if 0
	T wtmp[4][8];
	T m0, m1, m2, m3, s;
	T *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = cx.x;
	r0[1] = cx.y;
	r0[2] = cx.z;
	r0[3] = cx.w;
	r0[4] = T(1);
	r0[5] = r0[6] = r0[7] = T(0);

	r1[0] = cy.x;
	r1[1] = cy.y;
	r1[2] = cy.z;
	r1[3] = cy.w;
	r1[5] = T(1);
	r1[4] = r1[6] = r1[7] = T(0);

	r2[0] = cz.x;
	r2[1] = cz.y;
	r2[2] = cz.z;
	r2[3] = cz.w;
	r2[6] = T(1);
	r2[4] = r2[5] = r2[7] = T(0);

	r3[0] = cw.x;
	r3[1] = cw.y;
	r3[2] = cw.z;
	r3[3] = cw.w;
	r3[7] = T(1);
	r3[4] = r3[5] = r3[6] = T(0);

	// choose pivot - or die
	if (ax_abs(r3[0]) > ax_abs(r2[0])) ax_swap(r3, r2);
	if (ax_abs(r2[0]) > ax_abs(r1[0])) ax_swap(r2, r1);
	if (ax_abs(r1[0]) > ax_abs(r0[0])) ax_swap(r1, r0);
	if (T(0) == r0[0]) return makeIdentity();

	// eliminate first variable
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];

	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4]; if (s != T(0)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5]; if (s != T(0)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6]; if (s != T(0)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7]; if (s != T(0)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (ax_abs(r3[1]) > ax_abs(r2[1])) ax_swap(r3, r2);
	if (ax_abs(r2[1]) > ax_abs(r1[1])) ax_swap(r2, r1);
	if (T(0) == r1[1]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	// eliminate second variable
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];

	s = r1[4]; if (T(0) != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (T(0) != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (T(0) != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (T(0) != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (ax_abs(r3[2]) > ax_abs(r2[2])) ax_swap(r3, r2);
	if (T(0) == r2[2]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	// eliminate third variable
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3];
	r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5];
	r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	// last check
	if (T(0) == r3[3]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	s = T(1) / r3[3];    // now back substitute row 3
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;

	m2 = r2[3];          // now back substitute row 2
	s = T(1) / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2);
	r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2);
	r2[7] = s * (r2[7] - r3[7] * m2);

	m1 = r1[3];
	r1[4] -= r3[4] * m1;
	r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1;
	r1[7] -= r3[7] * m1;

	m0 = r0[3];
	r0[4] -= r3[4] * m0;
	r0[5] -= r3[5] * m0;
	r0[6] -= r3[6] * m0;
	r0[7] -= r3[7] * m0;

	m1 = r1[2];                 // now back substitute row 1
	s = T(1) / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1);
	r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1);
	r1[7] = s * (r1[7] - r2[7] * m1);

	m0 = r0[2];
	r0[4] -= r2[4] * m0;
	r0[5] -= r2[5] * m0;
	r0[6] -= r2[6] * m0;
	r0[7] -= r2[7] * m0;

	m0 = r0[1];                 // now back substitute row 0
	s = T(1) / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0);
	r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0);
	r0[7] = s * (r0[7] - r1[7] * m0);

	return This(r0[4], r0[5], r0[6], r0[7],
				r1[4], r1[5], r1[6], r1[7],
				r2[4], r2[5], r2[6], r2[7],
				r3[4], r3[5], r3[6], r3[7]);
#else
	T coef00 = cz.z * cw.w - cw.z * cz.w;
	T coef02 = cy.z * cw.w - cw.z * cy.w;
	T coef03 = cy.z * cz.w - cz.z * cy.w;

	T coef04 = cz.y * cw.w - cw.y * cz.w;
	T coef06 = cy.y * cw.w - cw.y * cy.w;
	T coef07 = cy.y * cz.w - cz.y * cy.w;

	T coef08 = cz.y * cw.z - cw.y * cz.z;
	T coef10 = cy.y * cw.z - cw.y * cy.z;
	T coef11 = cy.y * cz.z - cz.y * cy.z;

	T coef12 = cz.x * cw.w - cw.x * cz.w;
	T coef14 = cy.x * cw.w - cw.x * cy.w;
	T coef15 = cy.x * cz.w - cz.x * cy.w;

	T coef16 = cz.x * cw.z - cw.x * cz.z;
	T coef18 = cy.x * cw.z - cw.x * cy.z;
	T coef19 = cy.x * cz.z - cz.x * cy.z;

	T coef20 = cz.x * cw.y - cw.x * cz.y;
	T coef22 = cy.x * cw.y - cw.x * cy.y;
	T coef23 = cy.x * cz.y - cz.x * cy.y;

	Vec4 fac0(coef00, coef00, coef02, coef03);
	Vec4 fac1(coef04, coef04, coef06, coef07);
	Vec4 fac2(coef08, coef08, coef10, coef11);
	Vec4 fac3(coef12, coef12, coef14, coef15);
	Vec4 fac4(coef16, coef16, coef18, coef19);
	Vec4 fac5(coef20, coef20, coef22, coef23);

	Vec4 v0(cy.x, cx.x, cx.x, cx.x);
	Vec4 v1(cy.y, cx.y, cx.y, cx.y);
	Vec4 v2(cy.z, cx.z, cx.z, cx.z);
	Vec4 v3(cy.w, cx.w, cx.w, cx.w);

	Vec4 signA(+1, -1, +1, -1);
	Vec4 signB(-1, +1, -1, +1);

	Vec4 inv0(v1 * fac0 - v2 * fac1 + v3 * fac2);
	Vec4 inv1(v0 * fac0 - v2 * fac3 + v3 * fac4);
	Vec4 inv2(v0 * fac1 - v1 * fac3 + v3 * fac5);
	Vec4 inv3(v0 * fac2 - v1 * fac4 + v2 * fac5);

	Mat4 inv(inv0 * signA,
			 inv1 * signB,
			 inv2 * signA,
			 inv3 * signB);

	Vec4 dot0(cx * inv.row(0));
	T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);
	T oneOverDeterminant = T(1) / dot1;

	return inv * oneOverDeterminant;
#endif
}

template<class T>
axMat4<T> axMat4<T>::inverse3x3() const {
	T oneOverDeterminant = T(1) / (
		+ cx.x * (cy.y * cz.z - cz.y * cy.z)
		- cy.x * (cx.y * cz.z - cz.y * cx.z)
		+ cz.x * (cx.y * cy.z - cy.y * cx.z));

	return This(
		(cy.y * cz.z - cz.y * cy.z) *  oneOverDeterminant, // xx
		(cx.y * cz.z - cz.y * cx.z) * -oneOverDeterminant, // xy
		(cx.y * cy.z - cy.y * cx.z) *  oneOverDeterminant, // xz
		0,
		(cy.x * cz.z - cz.x * cy.z) * -oneOverDeterminant, // yx
		(cx.x * cz.z - cz.x * cx.z) *  oneOverDeterminant, // yy
		(cx.x * cy.z - cy.x * cx.z) * -oneOverDeterminant, // yz
		0,
		(cy.x * cz.y - cz.x * cy.y) *  oneOverDeterminant, // zx
		(cx.x * cz.y - cz.x * cx.y) * -oneOverDeterminant, // zy
		(cx.x * cy.y - cy.x * cx.y) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T>
axMat4<T> axMat4<T>::inverse3x3Transpose() const {
	T oneOverDeterminant = T(1) / (
		+ cx.x * (cy.y * cz.z - cz.y * cy.z)
		- cy.x * (cx.y * cz.z - cz.y * cx.z)
		+ cz.x * (cx.y * cy.z - cy.y * cx.z));

	return This(
		(cy.y * cz.z - cz.y * cy.z) *  oneOverDeterminant, // xx
		(cy.x * cz.z - cz.x * cy.z) * -oneOverDeterminant, // yx
		(cy.x * cz.y - cz.x * cy.y) *  oneOverDeterminant, // zx
		0,
		(cx.y * cz.z - cz.y * cx.z) * -oneOverDeterminant, // xy
		(cx.x * cz.z - cz.x * cx.z) *  oneOverDeterminant, // yy
		(cx.x * cz.y - cz.x * cx.y) * -oneOverDeterminant, // zy
		0,
		(cx.y * cy.z - cy.y * cx.z) *  oneOverDeterminant, // xz
		(cx.x * cy.z - cy.x * cx.z) * -oneOverDeterminant, // yz
		(cx.x * cy.y - cy.x * cx.y) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T>
axMat4<T> axMat4<T>::operator* (const This & r) const {
	This o;
	T e0, e1, e2, e3;

	e0 = cx.x, e1 = cy.x, e2 = cz.x, e3 = cw.x;
	o.cx.x = e0*r.cx.x + e1*r.cx.y + e2*r.cx.z + e3*r.cx.w;
	o.cy.x = e0*r.cy.x + e1*r.cy.y + e2*r.cy.z + e3*r.cy.w;
	o.cz.x = e0*r.cz.x + e1*r.cz.y + e2*r.cz.z + e3*r.cz.w;
	o.cw.x = e0*r.cw.x + e1*r.cw.y + e2*r.cw.z + e3*r.cw.w;

	e0 = cx.y, e1 = cy.y, e2 = cz.y, e3 = cw.y;
	o.cx.y = e0*r.cx.x + e1*r.cx.y + e2*r.cx.z + e3*r.cx.w;
	o.cy.y = e0*r.cy.x + e1*r.cy.y + e2*r.cy.z + e3*r.cy.w;
	o.cz.y = e0*r.cz.x + e1*r.cz.y + e2*r.cz.z + e3*r.cz.w;
	o.cw.y = e0*r.cw.x + e1*r.cw.y + e2*r.cw.z + e3*r.cw.w;

	e0 = cx.z, e1 = cy.z, e2 = cz.z, e3 = cw.z;
	o.cx.z = e0*r.cx.x + e1*r.cx.y + e2*r.cx.z + e3*r.cx.w;
	o.cy.z = e0*r.cy.x + e1*r.cy.y + e2*r.cy.z + e3*r.cy.w;
	o.cz.z = e0*r.cz.x + e1*r.cz.y + e2*r.cz.z + e3*r.cz.w;
	o.cw.z = e0*r.cw.x + e1*r.cw.y + e2*r.cw.z + e3*r.cw.w;

	e0 = cx.w, e1 = cy.w, e2 = cz.w, e3 = cw.w;
	o.cx.w = e0*r.cx.x + e1*r.cx.y + e2*r.cx.z + e3*r.cx.w;
	o.cy.w = e0*r.cy.x + e1*r.cy.y + e2*r.cy.z + e3*r.cy.w;
	o.cz.w = e0*r.cz.x + e1*r.cz.y + e2*r.cz.z + e3*r.cz.w;
	o.cw.w = e0*r.cw.x + e1*r.cw.y + e2*r.cw.z + e3*r.cw.w;

	return o;
}

template<class T>
axMat4<T> axMat4<T>::s_perspective(T fovy_rad, T aspect, T zNear, T zFar) {
	AX_ASSERT(!ax_math_equals0(aspect));

	T deltaZ = zFar - zNear;
	T tf = ax_tan(fovy_rad / T(2));
		
	return This(
		1 / (aspect * tf), 0,      0,                           0,
		0,                 1 / tf, 0,                           0,
		0,                 0,      -(zFar + zNear) / deltaZ,   -1,
		0,                 0,      -2 * zNear * zFar / deltaZ,  0
	);
}

template<class T>
axMat4<T> axMat4<T>::s_ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
	T w = right - left;
	T h = top - bottom;
	T d = zFar - zNear;

	if( w == 0 || h == 0 || d == 0 ) {
		return kIdentity();
	}

	return This(
		2/w, 0,    0,   0,
		0,   2/h,  0,   0,
		0,   0,   -2/d, 0,
		-(right+left) / w, -(top+bottom) / h, -(zFar+zNear) / d, 1
	);
}

template<class T>
axMat4<T> axMat4<T>::s_lookAt(const Vec3 & eye, const Vec3 & aim, const Vec3 & up) {
	auto f = (aim - eye).normal();
	auto s = f.cross(up).normal();
	auto u = s.cross(f);

	return This(
		s.x,  u.x, -f.x, 0,
		s.y,  u.y, -f.y, 0,
		s.z,  u.z, -f.z, 0,
		-s.dot(eye), -u.dot(eye), f.dot(eye), 1
	);
}

template<class T>
axMat4<T> axMat4<T>::s_rotate(const Vec3& rad) {
	if (rad.equals0()) return kIdentity();

	Vec3 s, c;
	ax_sincos(rad.x, s.x, c.x);
	ax_sincos(rad.y, s.y, c.y);
	ax_sincos(rad.z, s.z, c.z);

	return This(
		(c.y*c.z), (s.x*s.y*c.z - c.x*s.z), (s.x*s.z + c.x*s.y*c.z), 0,
		(c.y*s.z), (c.x*c.z + s.x*s.y*s.z), (c.x*s.y*s.z - s.x*c.z), 0,
		(-s.y),    (s.x*c.y),               (c.x*c.y),               0,
		0,         0,                        0,                      1
	);
}

template<class T>
axMat4<T> axMat4<T>::s_rotateX(T rad) {
	if (rad == 0) return kIdentity();

	T s, c;
	ax_sincos(rad, s, c);
	return This( 1, 0, 0, 0,
				 0, c, s, 0,
				 0,-s, c, 0,
				 0, 0, 0, 1);
}

template<class T>
axMat4<T> axMat4<T>::s_rotateY(T rad) {
	if (rad == 0) return kIdentity();

	T s, c;
	ax_sincos(rad, s, c);
	return This( c, 0,-s, 0,
				 0, 1, 0, 0,
				 s, 0, c, 0,
				 0, 0, 0, 1);
}

template<class T>
axMat4<T> axMat4<T>::s_rotateZ(T rad) {
	if (rad == 0) return kIdentity();

	T s, c;
	ax_sincos(rad, s, c);
	return This( c, s, 0, 0,
				-s, c, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1);
}

template<class T>
axMat4<T> axMat4<T>::s_translateScale(const Vec3 & translate, const Vec3 & scale) {
	return This( scale.x, 0, 0, 0,
				 0, scale.y, 0, 0,
				 0, 0, scale.z, 0,
				 translate.x, translate.y, translate.z, 1);
}

template<class T>
axMat4<T> axMat4<T>::s_TRS(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale) {
	Vec3 s, c;
	ax_sincos(rotate.x, s.x, c.x);
	ax_sincos(rotate.y, s.y, c.y);
	ax_sincos(rotate.z, s.z, c.z);

	return This(scale.x * (c.y*c.z),				scale.x * (c.y*s.z),               scale.x * (-s.y),    0,
				scale.y * (s.x*s.y*c.z - c.x*s.z),	scale.y * (c.x*c.z + s.x*s.y*s.z), scale.y * (s.x*c.y), 0,
				scale.z * (s.x*s.z + c.x*s.y*c.z),	scale.z * (c.x*s.y*s.z - s.x*c.z), scale.z * (c.x*c.y), 0,
				translate.x, translate.y, translate.z, 1);
}

template <class T>
axMat4<T> axMat4<T>::s_translate(const Vec3& v) {
	return This( 1,   0,   0,   0,
				 0,   1,   0,   0,
				 0,   0,   1,   0,
				 v.x, v.y, v.z, 1);
}

template <class T>
axMat4<T> axMat4<T>::s_scale(const Vec3& s) {
	return This( s.x, 0,   0,   0,
				 0,   s.y, 0,   0,
				 0,   0,   s.z, 0,
				 0,   0,   0,   1);
}

template <class T>
axMat4<T> axMat4<T>::s_shear(const Vec3& v) {
	const T &xy = v.x;
	const T &xz = v.y;
	const T &yz = v.z;
	return This( 1,  0,  0, 0,
				 xy, 1,  0, 0,
				 xz, yz, 1, 0,
				 0,  0,  0, 1);
}

#define	AX_MACRO_OP(T)	\
	template class axMat4<T>; \
	AX_STATIC_ASSERT(sizeof(axMat4<T>) == sizeof(T) * axMat4<T>::kElementCount); \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP

