#pragma once

#include "axVec2.h"
#include "axRect2.h"

template<class T>
class axVec3 {
public:
	using Element = T;
	static const axInt kElementCount = 3;

	using Vec2   = axVec2<T>;
	using Vec3   = axVec3<T>;
	using Vec4   = axVec4<T>;
	using Tuple2 = axTuple2<T>;
	using Tuple3 = axTuple3<T>;
	using Tuple4 = axTuple4<T>;
	using Rect2  = axRect2<T>;
	using Mat4   = axMat4<T>;
	using Quat4  = axQuat4<T>;

	T x, y, z;

	AX_STRUCT_BEGIN(axVec3<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(x, y, z);
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_IO(se, x,y,z);
	}

	AX_INLINE explicit constexpr axVec3() = default;
	AX_INLINE explicit constexpr axVec3(T v) : x(v), y(v), z(v) {}
	AX_INLINE			constexpr axVec3(T x_, T y_, T z_) :x(x_), y(y_), z(z_) {}
	AX_INLINE			constexpr axVec3(const axVec2<T>& v, T z_) : x(v.x), y(v.y), z(z_) {}
	AX_INLINE			constexpr axVec3(const Tuple3& t) { operator=(t); }

	AX_INLINE		 T* data()			{ return &x; }
	AX_INLINE const T* data() const	{ return &x; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(&x, kElementCount); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(&x, kElementCount); }

	AX_INLINE constexpr void set(T x_, T y_, T z_)					{ x = x_;  y = y_;  z = z_; }
	AX_INLINE constexpr void set(const axVec2<T>& v, T z_)			{ set(v.x, v.y, z_); }
	AX_INLINE constexpr void setToDefaultValue()					{ set(0,0,0); }

	AX_INLINE void operator=(const Tuple3& t)				{ set(t.x, t.y, t.z); }

	AX_NODISCARD AX_INLINE constexpr Vec3 xyz() const { return Vec3(x, y, z); }
	AX_NODISCARD AX_INLINE constexpr Vec3 xzy() const { return Vec3(x, z, y); }
	AX_NODISCARD AX_INLINE constexpr Vec3 yxz() const { return Vec3(y, x, z); }
	AX_NODISCARD AX_INLINE constexpr Vec3 yzx() const { return Vec3(y, z, x); }
	AX_NODISCARD AX_INLINE constexpr Vec3 zxy() const { return Vec3(z, x, y); }
	AX_NODISCARD AX_INLINE constexpr Vec3 zyx() const { return Vec3(z, y, x); }

	AX_NODISCARD AX_INLINE constexpr Vec2 xy() const	{ return axVec2<T>(x, y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 xz() const	{ return axVec2<T>(x, z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yx() const	{ return axVec2<T>(y, x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yz() const	{ return axVec2<T>(y, z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zx() const	{ return axVec2<T>(z, x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zy() const	{ return axVec2<T>(z, y); }

	AX_NODISCARD AX_INLINE constexpr Vec4 xyz1() const;

	AX_NODISCARD AX_INLINE constexpr This operator -  () const						{ return This(-x, -y, -z); }

	AX_NODISCARD AX_INLINE constexpr This operator +	(const This& r) const		{ return This(x + r.x, y + r.y, z + r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator -	(const This& r) const		{ return This(x - r.x, y - r.y, z - r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator *	(const This& r) const		{ return This(x * r.x, y * r.y, z * r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator /	(const This& r) const		{ return This(x / r.x, y / r.y, z / r.z); }

	AX_NODISCARD AX_INLINE constexpr This operator +	(T s) const					{ return This(x + s, y + s, z + s); }
	AX_NODISCARD AX_INLINE constexpr This operator -	(T s) const					{ return This(x - s, y - s, z - s); }
	AX_NODISCARD AX_INLINE constexpr This operator *	(T s) const					{ return This(x * s, y * s, z * s); }
	AX_NODISCARD AX_INLINE constexpr This operator /	(T s) const					{ return This(x / s, y / s, z / s); }
								 
	AX_INLINE				constexpr void operator +=(T s)							{ x += s; y += s; z += s; }
	AX_INLINE				constexpr void operator -=(T s)							{ x -= s; y -= s; z -= s; }
	AX_INLINE				constexpr void operator *=(T s)							{ x *= s; y *= s; z *= s; }
	AX_INLINE				constexpr void operator /=(T s)							{ x /= s; y /= s; z /= s; }

	AX_INLINE				constexpr void operator +=(const This& r)				{ x += r.x; y += r.y; z += r.z; }
	AX_INLINE				constexpr void operator -=(const This& r)				{ x -= r.x; y -= r.y; z -= r.z; }
	AX_INLINE				constexpr void operator *=(const This& r)				{ x *= r.x; y *= r.y; z *= r.z; }
	AX_INLINE				constexpr void operator /=(const This& r)				{ x /= r.x; y /= r.y; z /= r.z; }

	AX_NODISCARD AX_INLINE constexpr This operator * (const Quat4& q) const;
	AX_INLINE				constexpr void operator *=(const Quat4& q)				{ *this = *this * q; }

	AX_NODISCARD AX_INLINE	constexpr T	 lengthSq	() const						{ return x * x + y * y + z * z; }
	AX_NODISCARD AX_INLINE	constexpr T	 length		() const						{ return ax_sqrt (lengthSq()); }
	AX_NODISCARD AX_INLINE constexpr T	 invLength	() const						{ return ax_rsqrt(lengthSq()); }

	AX_NODISCARD AX_INLINE	constexpr T	 distance	(const This &r) const			{ return (*this - r).length(); }
	AX_NODISCARD AX_INLINE constexpr T	 distanceSq	(const This &r) const			{ return (*this - r).lengthSq(); }
									 
	AX_NODISCARD AX_INLINE	constexpr bool equals		(const Vec3& r, T ep = ax_epsilon<T>()) const;
	AX_NODISCARD AX_INLINE	constexpr bool equals0	(T ep = ax_epsilon<T>()) const;
									 
	AX_NODISCARD AX_INLINE	constexpr bool isParallel	(const Vec3& r) const		{ return cross(r).equals0(); }
									 
	AX_NODISCARD AX_INLINE	constexpr This normal		() const;
	AX_INLINE				constexpr void normalize	()							{ *this = normal(); }
									 
	AX_NODISCARD AX_INLINE	constexpr This cross		(const This& v) const		{ return This(y*v.z - z*v.y, z*v.x - x*v.z,x*v.y - y*v.x); }
	AX_NODISCARD AX_INLINE	constexpr T	 dot		(const This& v) const			{ return (x*v.x) + (y*v.y) + (z*v.z); }

	AX_NODISCARD AX_INLINE constexpr bool operator==	(const This& r) const		{ return x == r.x && y == r.y && z == r.z; }
	AX_NODISCARD AX_INLINE constexpr bool operator!=	(const This& r) const		{ return x != r.x || y != r.y || z != r.z; }

	template<class V> AX_INLINE constexpr 
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE constexpr 
	static Vec3 s_cast(const V& v) { return This( 	static_cast<T>(v.x), 
													static_cast<T>(v.y),
													static_cast<T>(v.z)); }

	static This s_unprojectInvMatrix(const Vec3& screenPos, const Mat4& invMat, const Rect2& viewport);
private:
};

using axVec3i = axVec3< axInt >;
using axVec3f = axVec3< axFloat32 >;
using axVec3d = axVec3< axFloat64 >;

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axVec3<T> > { static const bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axVec3<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axVec3<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axVec3<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axVec3<T>& value) { se.reader.readValue(value); }


template<class T> AX_INLINE constexpr 
bool axVec3<T>::equals0(T ep) const {
	return ax_math_equals0(x, ep)
		&& ax_math_equals0(y, ep)
		&& ax_math_equals0(z, ep);
}

template<class T> AX_INLINE constexpr 
bool axVec3<T>::equals(const Vec3& r, T ep) const {
	return ax_math_equals(x, r.x, ep)
		&& ax_math_equals(y, r.y, ep)
		&& ax_math_equals(z, r.z, ep);
}

template<class T> AX_INLINE constexpr 
axVec3<T> axVec3<T>::normal() const {
	return *this * invLength();
}

template<class T> AX_INLINE constexpr axVec3<T> operator+(T s, const axVec3<T>& v) { return axVec3<T>(s + v._v); }
template<class T> AX_INLINE constexpr axVec3<T> operator-(T s, const axVec3<T>& v) { return axVec3<T>(s - v._v); }
template<class T> AX_INLINE constexpr axVec3<T> operator*(T s, const axVec3<T>& v) { return axVec3<T>(s * v._v); }
template<class T> AX_INLINE constexpr axVec3<T> operator/(T s, const axVec3<T>& v) { return axVec3<T>(s / v._v); }

template<class T> AX_INLINE constexpr 
void ax_sincos(const axVec3<T>& rad, axVec3<T>& s, axVec3<T>& c) {
	ax_sincos(rad.x, s.x, c.x);
	ax_sincos(rad.y, s.y, c.y);
	ax_sincos(rad.z, s.z, c.z);
}

// 1 arg
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec3<T> FUNC(const axVec3<T>& a) { \
		return axVec3<T>( \
			FUNC(a.x), \
			FUNC(a.y), \
			FUNC(a.z)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_abs)
	AX_MACRO_OP(ax_floor)
	AX_MACRO_OP(ax_ceil)
	AX_MACRO_OP(ax_round)
	AX_MACRO_OP(ax_degrees)
	AX_MACRO_OP(ax_radians)
	AX_MACRO_OP(ax_clamp01)
#undef AX_MACRO_OP
		
// 2 args
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec3<T> FUNC(const axVec3<T>& a, const axVec3<T>& b) { \
		return axVec3<T>( \
			FUNC(a.x, b.x), \
			FUNC(a.y, b.y), \
			FUNC(a.z, b.z)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_min)
	AX_MACRO_OP(ax_max)
	AX_MACRO_OP(ax_fmod)
	AX_MACRO_OP(ax_multiple_of)
#undef AX_MACRO_OP

// 3 args
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec3<T> FUNC(const axVec3<T>& a, const axVec3<T>& b, const axVec3<T>& c) { \
		return axVec3<T>( \
			FUNC(a.x, b.x, c.x), \
			FUNC(a.y, b.y, c.y), \
			FUNC(a.z, b.z, c.z)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_clamp)
#undef AX_MACRO_OP

// to_int
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec3i	FUNC(const axVec3<T>& v) { \
		return axVec3i(FUNC(v.x), FUNC(v.y), FUNC(v.z)); \
	} \
//-----
	AX_MACRO_OP(ax_trunc_to_int)
	AX_MACRO_OP(ax_round_to_int)
	AX_MACRO_OP(ax_ceil_to_int)
	AX_MACRO_OP(ax_floor_to_int)
#undef AX_MACRO_OP

AX_INLINE constexpr 
axVec3f ax_lerp(const axVec3f & a, const axVec3f & b, float weight) {
	return axVec3f(
		ax_lerp(a.x, b.x, weight),
		ax_lerp(a.y, b.y, weight),
		ax_lerp(a.z, b.z, weight)
	);
}

template<class T> AX_INLINE constexpr void axTuple3<T>::operator=(const Vec3& v) { x = v.x; y = v.y; z = v.z; }
template<class T> AX_INLINE constexpr axVec3<T> axVec2<T>::xy0() const { return Vec3(x,y,0); }
template<class T> AX_INLINE constexpr axVec3<T> axVec2<T>::xy1() const { return Vec3(x,y,1); }
template<class T> AX_INLINE constexpr axVec3<T> axVec2<T>::x0y() const { return Vec3(x,0,y); }
template<class T> AX_INLINE constexpr axVec3<T> axVec2<T>::x1y() const { return Vec3(x,1,y); }
