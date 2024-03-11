#pragma once

#include "axVec4.h"

// column major matrix
template<class T>
class axMat4 {
public:
	using Vec2		= axVec2<T>;
	using Vec3		= axVec3<T>;
	using Vec4		= axVec4<T>;
	using Quat4		= axQuat4<T>;
	using Mat4		= axMat4<T>;
	using Tuple3	= axTuple3<T>;
	using Tuple4	= axTuple4<T>;

	static const axInt kElementCount = 16;

	union {
		struct{ Vec4 cx, cy, cz, cw; };
		T data[kElementCount];
	};

	AX_STRUCT_BEGIN(axMat4<T>, axNoBase)
		ax_FIELDS(cx, cy, cz, cw)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	explicit axMat4() = default;
	explicit axMat4(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_);

	axMat4(	T xx, T xy, T xz, T xw,
			T yx, T yy, T yz, T yw,
			T zx, T zy, T zz, T zw,
			T wx, T wy, T wz, T ww );

	void set(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_);
	void set( T xx, T xy, T xz, T xw,
			  T yx, T yy, T yz, T yw,
			  T zx, T zy, T zz, T zw,
			  T wx, T wy, T wz, T ww);

	AX_INLINE void setToDefaultValue() { setIdentity(); }

	Vec4 col(axInt i) const	{ return (&cx)[i]; }
	Vec4 row(axInt i) const	{ return Vec4(cx.data()[i], cy.data()[i], cz.data()[i], cw.data()[i]); }

	static	This kIdentity			();
			void setIdentity		()	{ *this = kIdentity(); }

	static	This s_perspective		(T fovy_rad, T aspect, T zNear, T zFar);
	static	This s_ortho			(T left, T right, T bottom, T top, T zNear, T zFar);
	static	This s_lookAt			(const Vec3 & eye, const Vec3 & aim, const Vec3 & up);

	static	This s_translate		(const Vec3& v);
	static	This s_translate		(const Vec2& v)			{ return s_translate(Vec3(v, 0)); }

	static	This s_rotate			(const Vec3 & v);
	static	This s_rotateX			(T rad);
	static	This s_rotateY			(T rad);
	static	This s_rotateZ			(T rad);

	static	This s_rotateDeg		(const Vec3 & v)		{ return s_rotate(ax_radians(v)); }
	static	This s_rotateDegX		(T deg)					{ return s_rotateX(ax_radians(deg)); }
	static	This s_rotateDegY		(T deg)					{ return s_rotateY(ax_radians(deg)); }
	static	This s_rotateDegZ		(T deg)					{ return s_rotateZ(ax_radians(deg)); }

	static	This s_scale			(T s)					{ return s_scale({s,s,s}); }
	static	This s_scale			(const Vec2 & v)		{ return s_scale(Vec3(v, 1)); }
	static	This s_scale			(const Vec3 & v);

	static	This s_shear			(const Vec3 & v);

	static	This s_TRS				(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale);
	static	This s_TRS_Deg			(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale) { return s_TRS(translate, ax_radians(rotate), scale); }

	static	This s_translateScale	(const Vec3 & translate, const Vec3 & scale);
	static	This s_translateScale	(const Vec3 & translate, const T &    scale) { return s_translateScale(translate, Vec3(scale)); }

			This transpose			() const;
			This inverse			() const;
			This inverse3x3			() const;
			This inverse3x3Transpose() const;

			bool operator==			(const This &r) const	{ return cx == r.cx && cy == r.cy && cw == r.cw && cz == r.cz; }
			bool operator!=			(const This &r) const	{ return cx != r.cx || cy != r.cy || cw != r.cw || cz != r.cz; }

			bool equals				(const This &r, const T& ep = ax_epsilon<T>()) const;

			This operator+			(const T& s) const		{ return This(cx + s, cy + s, cz + s, cw + s); }
			This operator-			(const T& s) const		{ return This(cx - s, cy - s, cz - s, cw - s); }
			This operator*			(const T& s) const		{ return This(cx * s, cy * s, cz * s, cw * s); }
			This operator/			(const T& s) const		{ return This(cx / s, cy / s, cz / s, cw / s); }

			void operator+=			(const T& s)			{ cx += s; cy += s; cz += s; cw += s; }
			void operator-=			(const T& s)			{ cx -= s; cy -= s; cz -= s; cw -= s; }
			void operator*=			(const T& s)			{ cx *= s; cy *= s; cz *= s; cw *= s; }
			void operator/=			(const T& s)			{ cx /= s; cy /= s; cz /= s; cw /= s; }

			This operator*			(const This &r) const;
			void operator*=			(const This &r)			{ *this = *this * r; }

			Vec4	mulPoint		(const Vec4&   v) const;
			Tuple4	mulPoint		(const Tuple4& v) const;
//			Vec4 operator*			(const Vec4& v) const	{ return mulPoint(v); }

			Vec3	mulPoint		(const Vec3&   v) const;
			Tuple3	mulPoint		(const Tuple3& v) const;

			// faster than mulPoint but no projection
			Vec3 mulPoint4x3		(const Vec3& v) const;

			// for vector (direction)
			Vec3 mulVector			(const Vec3& v) const;

			// for normal non-uniform scale
			Vec3 mulNormal			(const Vec3& v) const;

			Vec3 dirX() const { return Vec3(cx.x, cy.x, cz.x).normal(); }
			Vec3 dirY() const { return Vec3(cx.y, cy.y, cz.y).normal(); }
			Vec3 dirZ() const { return Vec3(cx.z, cy.z, cz.z).normal(); }

			Vec3 position() const { return cw.xyz(); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axMat4<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

//---------- inline ---------------

template<class T> inline
axMat4<T>::axMat4(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_)
	: cx(cx_), cy(cy_), cz(cz_), cw(cw_) 
{
}

template<class T> inline
void axMat4<T>::set(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_) {
	cx = cx_;
	cy = cy_;
	cz = cz_;
	cw = cw_;
}

template<class T> inline
void axMat4<T>::set(T xx, T xy, T xz, T xw,
					T yx, T yy, T yz, T yw,
					T zx, T zy, T zz, T zw,
					T wx, T wy, T wz, T ww)
{
	cx.set(xx, xy, xz, xw);
	cy.set(yx, yy, yz, yw);
	cz.set(zx, zy, zz, zw);
	cw.set(wx, wy, wz, ww);
}

template<class T> inline
axMat4<T>::axMat4(	T xx, T xy, T xz, T xw, 
					T yx, T yy, T yz, T yw, 
					T zx, T zy, T zz, T zw, 
					T wx, T wy, T wz, T ww) 
{
	cx.x = xx; cx.y = xy; cx.z = xz; cx.w = xw;
	cy.x = yx; cy.y = yy; cy.z = yz; cy.w = yw;
	cz.x = zx; cz.y = zy; cz.z = zz; cz.w = zw;
	cw.x = wx; cw.y = wy; cw.z = wz; cw.w = ww;
}


template<class T> inline
axMat4<T> axMat4<T>::kIdentity() {
	return This(1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1);
}


template<class T> inline
bool axMat4<T>::equals(const This &r, const T& ep) const {
	return cx.equals(r.cx, ep)
		&& cy.equals(r.cy, ep)
		&& cw.equals(r.cw, ep)
		&& cz.equals(r.cz, ep);
}

template<class T> inline
axVec4<T> axMat4<T>::mulPoint(const Vec4& v) const {
	return Vec4(cx * v.x 
			  + cy * v.y 
			  + cz * v.z 
			  + cw * v.w);
}

template<class T> inline
axTuple4<T> axMat4<T>::mulPoint(const Tuple4& v) const {
	return Tuple4(mulPoint(Vec4(v)));
}

template<class T> inline
axVec3<T> axMat4<T>::mulPoint(const Vec3& v) const {
	return mulPoint(Vec4(v, 1)).toVec3();
}

template<class T> inline
axTuple3<T> axMat4<T>::mulPoint(const Tuple3& v) const {
	return Tuple3(mulPoint(Vec3(v)));
}

template<class T> inline
axVec3<T> axMat4<T>::mulPoint4x3(const Vec3& v) const {
	return mulPoint(Vec4(v, 1)).xyz();
}

template<class T> inline
axVec3<T> axMat4<T>::mulVector(const Vec3& v) const {
	return Vec3(cx.xyz() * v.x 
			  + cy.xyz() * v.y 
			  + cz.xyz() * v.z);
}

template<class T> inline
axVec3<T> axMat4<T>::mulNormal(const Vec3& v) const {
	return inverse3x3Transpose().mulVector(v);
}

//template<class T> inline
//axVec4<T> axVec4<T>::operator*(const axMat4<T>& m) const {
//	return axVec4<T>(
//		x * m.cx.x + y * m.cx.y + z * m.cx.z + w * m.cx.w,
//		x * m.cy.x + y * m.cy.y + z * m.cy.z + w * m.cy.w,
//		x * m.cz.x + y * m.cz.y + z * m.cz.z + w * m.cz.w,
//		x * m.cw.x + y * m.cw.y + z * m.cw.z + w * m.cw.w
//	);
//}

template<class T> inline
axMat4<T> axMat4<T>::transpose() const {
	return This(
		cx.x, cy.x, cz.x, cw.x,
		cx.y, cy.y, cz.y, cw.y,
		cx.z, cy.z, cz.z, cw.z,
		cx.w, cy.w, cz.w, cw.w
	);
}

using axMat4f = axMat4< axFloat32 >;
using axMat4d = axMat4< axFloat64 >;

// extern template class axMat4<axFloat32>;
// extern template class axMat4<axFloat64>;
