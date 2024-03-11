#pragma once

#include "axMat4.h"

// Quaternion
template<class T>
class axQuat4 {
public:
	using Element = T;
	static const axInt kElementCount = 4;

	using Vec3 = axVec3<T>;
	using Vec4 = axVec4<T>;
	using Mat4 = axMat4<T>;

	union {
		struct{T x, y, z, w;};
		T data[kElementCount];
	};

	AX_STRUCT_BEGIN(axQuat4<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(x, y, z, w)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	explicit constexpr axQuat4() = default;
	explicit constexpr axQuat4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}

	constexpr void set(T x_, T y_, T z_, T w_) { x=x_; y=y_; z=z_; w=w_; }

	static	This s_identity() { return This(0,0,0,1); }
			void setIdentity()  { *this = s_identity(); }

	static	This s_angleAxis(T rad, const Vec3& axis);
			void setAngleAxis(T rad, const Vec3& axis) { *this = s_angleAxis(rad, axis); }

			T	 angle() const { return ax_acos(w) * T(2); }
			Vec3 axis () const;

	static	This s_euler(const Vec3& r);
	static	This s_eulerX(T rad)	{ T s, c; ax_sincos(rad * T(0.5), s, c); return This(s,0,0,c); }
	static	This s_eulerY(T rad)	{ T s, c; ax_sincos(rad * T(0.5), s, c); return This(0,s,0,c); }
	static	This s_eulerZ(T rad)	{ T s, c; ax_sincos(rad * T(0.5), s, c); return This(0,0,s,c); }

	static	This s_eulerDegX(T deg)	{ return s_eulerX(ax_radians(deg)); }
	static	This s_eulerDegY(T deg)	{ return s_eulerY(ax_radians(deg)); }
	static	This s_eulerDegZ(T deg)	{ return s_eulerZ(ax_radians(deg)); }

			void setEuler(const Vec3& r)	{ *this = s_euler(r); }
			void setEulerX(T rad) { *this = s_eulerX(rad); }
			void setEulerY(T rad) { *this = s_eulerY(rad); }
			void setEulerZ(T rad) { *this = s_eulerZ(rad); }

			void setEulerDegX(T deg) { *this = s_eulerDegX(deg); }
			void setEulerDegY(T deg) { *this = s_eulerDegY(deg); }
			void setEulerDegZ(T deg) { *this = s_eulerDegZ(deg); }

			Vec3 euler() const	{ return Vec3(eulerX(), eulerY(), eulerZ()); }
			T eulerX() const;
			T eulerY() const;
			T eulerZ() const;

	This conjugate() const;
	This inverse() const;

			T dot(const This& r) const;

	This operator*(const This& r) const;
	Vec3 operator*(const Vec3& v) const;

	This operator+(const This& r) const { return This(x+r.x, y+r.y, z+r.z, w+r.w); }
	This operator-(const This& r) const { return This(x-r.x, y-r.y, z-r.z, w-r.w); }

	This operator*(const T&    v) const { return This(x*v, y*v, z*v, w*v); }
	This operator/(const T&    v) const { return This(x/v, y/v, z/v, w/v); }

	This operator-() const { return This(-x, -y, -z, -w); }

	template<class V> AX_INLINE
	void setByCast(const V& v) { *this = s_cast(v); }	

	template<class V> AX_INLINE
	static This s_cast(const V& v) { return This(	static_cast<T>(v.x), 
													static_cast<T>(v.y),
													static_cast<T>(v.z),
													static_cast<T>(v.w)); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axQuat4<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> AX_INLINE
axQuat4<T> ax_lerp(const axQuat4<T> & a, const axQuat4<T> & b, T weight) {
	return axQuat4<T>(
		ax_lerp(a.x, b.x, weight),
		ax_lerp(a.y, b.y, weight),
		ax_lerp(a.z, b.z, weight),
		ax_lerp(a.w, b.w, weight)
	);
}

template<class T, bool LONG_WAY = false> AX_INLINE
axQuat4<T> ax_slerp(const axQuat4<T> & a, const axQuat4<T> & b, T weight) {
	axQuat4<T> z = b;
	T cosTheta = a.dot(b);

	AX_IF_CONSTEXPR (!LONG_WAY) {
		// If cosTheta < 0, the interpolation will take the long way around the sphere. 
		// To fix this, one quat must be negated.
		if (cosTheta < T(0)) {
			z        = -b;
			cosTheta = -cosTheta;
		}
	}

	// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
	if(cosTheta > T(1) - ax_epsilon<T>()) {
		// Linear interpolation
		return ax_lerp(a, z, weight);
	} else {
		// Essential Mathematics, page 467
		T angle = ax_acos(cosTheta);
		return (a * ax_sin((T(1) - weight) * angle) + z * ax_sin(weight * angle)) / ax_sin(angle);
	}
}

template<class T> AX_INLINE
axQuat4<T> ax_slerp_longway(const axQuat4<T> & a, const axQuat4<T> & b, T weight) {
	return ax_slerp<T, true>(a, b, weight);
}

template<class T> inline
T axQuat4<T>::dot(const This& r) const {
	return (x * r.x + y * r.y) + (z * r.z + w * r.w);
}

template<class T> inline
axVec3<T> axQuat4<T>::axis() const {
	T a = T(1) - w * w;
	if (a <= 0)
		return Vec3(0, 0, 1);
	T b = T(1) / ax_sqrt(a);
	return Vec3(x, y, z) * b;
}

template<class T> inline
axQuat4<T> axQuat4<T>::s_euler(const Vec3& r) {
	Vec3 s, c;
	ax_sincos(r * T(0.5), s, c);
	return This(
		s.x * c.y * c.z - c.x * s.y * s.z,
		c.x * s.y * c.z + s.x * c.y * s.z,
		c.x * c.y * s.z - s.x * s.y * c.z,
		c.x * c.y * c.z + s.x * s.y * s.z
	);
}

template<class T> inline
T axQuat4<T>::eulerZ() const {
	return ax_atan2(T(2) * (x * y + w * z), w * w + x * x - y * y - z * z);
}

template<class T> inline
T axQuat4<T>::eulerY() const {
	return ax_asin(ax_clamp(T(-2) * (x * z - w * y), T(-1), T(1)));
}

template<class T> inline
T axQuat4<T>::eulerX() const {
	return ax_atan2(T(2) * (y * z + w * x), w * w - x * x - y * y + z * z);
}

template<class T> inline
axQuat4<T> axQuat4<T>::conjugate() const {
	return This(-x, -y, -z, w);
}

template<class T> inline
axQuat4<T> axQuat4<T>::inverse() const {
	T d = x * x + y * y + z * z + w * w;
	return This(-x/d, -y/d, -z/d, w/d);
}

template<class T> constexpr 
axVec3<T> axVec3<T>::operator*(const Quat4& q) const {
	return q.inverse() * *this;
}

template<class T> inline
axVec3<T> axQuat4<T>::operator*(const Vec3& v) const {
	Vec3 qv(x, y, z);
	auto uv  = qv.cross(v);
	auto uuv = qv.cross(uv);
	return v + (uv * w + uuv) * T(2);
}

template<class T> inline
axQuat4<T> axQuat4<T>::operator*(const This& r) const {
	return This(x * r.w +  w * r.x + z * r.y - y * r.z,
				y * r.w +  w * r.y + x * r.z - z * r.x,
				z * r.w +  w * r.z + y * r.x - x * r.y,
				w * r.w -  x * r.x - y * r.y - z * r.z);
}

template<class T> inline
axQuat4<T> axQuat4<T>::s_angleAxis(T rad, const Vec3& axis) {
	T s, c;
	ax_sincos(rad * T(0.5), s, c);
	return This(axis.x * s, axis.y * s, axis.z * s, c);
}

using axQuat4f = axQuat4< axFloat32 >;
using axQuat4d = axQuat4< axFloat64 >;
