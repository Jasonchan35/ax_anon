#pragma once

#include "axVec3.h"

template<class T>
class alignas(16) axVec4 {
public:
	using Element = T;
	static const axInt kElementCount = 4;

	using Vec2   = axVec2<T>;
	using Vec3   = axVec3<T>;
	using Vec4   = axVec4<T>;
	using Tuple2 = axTuple2<T>;
	using Tuple3 = axTuple3<T>;
	using Tuple4 = axTuple4<T>;
	using Rect2  = axRect2<T>;
	using Mat4   = axMat4<T>;
	using Quat4  = axQuat4<T>;

	T x, y, z, w;

	AX_STRUCT_BEGIN(axVec4<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(x, y, z, w);
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	AX_INLINE explicit constexpr axVec4() = default;
	AX_INLINE explicit constexpr axVec4(T v) : x(v), y(v), z(v), w(v) {}
	AX_INLINE			constexpr axVec4(T v, T w_) : x(v), y(v), z(v), w(w_) {}
	AX_INLINE			constexpr axVec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
	AX_INLINE			constexpr axVec4(const axVec3<T>& v, T w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
	AX_INLINE			constexpr axVec4(const Tuple4& t) { operator=(t); }

	AX_INLINE		 T* data()			{ return &x; }
	AX_INLINE const T* data() const	{ return &x; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(&x, kElementCount); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(&x, kElementCount); }

	AX_INLINE constexpr void set(T x_, T y_, T z_, T w_)		{ x = x_;  y = y_;  z = z_;  w = w_;  }
	AX_INLINE constexpr void set(const axVec3<T>& v, T w_)		{ x = v.x; y = v.y; z = v.z; w = w_;  }
	AX_INLINE constexpr void setToDefaultValue()				{ set(0,0,0,1); }

	AX_INLINE constexpr void operator=(const Tuple4& t)		{ set(t.x, t.y, t.z, t.w); }

	AX_INLINE constexpr axVec2<T>	xy() const	{ return axVec2<T>(x,y); }
	AX_INLINE constexpr axVec2<T>	xz() const	{ return axVec2<T>(x,z); }
	AX_INLINE constexpr axVec2<T>	yz() const	{ return axVec2<T>(y,z); }
	AX_INLINE constexpr axVec3<T>	xyz() const	{ return axVec3<T>(x,y,z); }

	AX_INLINE constexpr axVec3<T>	homogenize () const { return (*this / w).xyz(); }
	AX_INLINE constexpr axVec3<T>	toVec3() const		{ return homogenize(); }

	AX_INLINE constexpr This	operator - () const						{ return This(-x, -y, -z, -w); }

	AX_INLINE constexpr This operator +	(const This& r) const		{ return This(x + r.x, y + r.y, z + r.z, w + r.w); }
	AX_INLINE constexpr This operator -	(const This& r) const		{ return This(x - r.x, y - r.y, z - r.z, w - r.w); }
	AX_INLINE constexpr This operator *	(const This& r) const		{ return This(x * r.x, y * r.y, z * r.z, w * r.w); }
	AX_INLINE constexpr This operator /	(const This& r) const		{ return This(x / r.x, y / r.y, z / r.z, w / r.w); }

	AX_INLINE constexpr This operator +	(T s) const					{ return This(x + s, y + s, z + s, w + s); }
	AX_INLINE constexpr This operator -	(T s) const					{ return This(x - s, y - s, z - s, w - s); }
	AX_INLINE constexpr This operator *	(T s) const					{ return This(x * s, y * s, z * s, w * s); }
	AX_INLINE constexpr This operator /	(T s) const					{ return This(x / s, y / s, z / s, w / s); }

	AX_INLINE constexpr void operator +=	(T s)						{ x += s; y += s; z += s; w += s; }
	AX_INLINE constexpr void operator -=	(T s)						{ x -= s; y -= s; z -= s; w -= s; }
	AX_INLINE constexpr void operator *=	(T s)						{ x *= s; y *= s; z *= s; w *= s; }
	AX_INLINE constexpr void operator /=	(T s)						{ x /= s; y /= s; z /= s; w /= s; }

	AX_INLINE constexpr void operator +=	(const This& r)				{ x += r.x; y += r.y; z += r.z; w += r.w; }
	AX_INLINE constexpr void operator -=	(const This& r)				{ x -= r.x; y -= r.y; z -= r.z; w -= r.w; }
	AX_INLINE constexpr void operator *=	(const This& r)				{ x *= r.x; y *= r.y; z *= r.z; w *= r.w; }
	AX_INLINE constexpr void operator /=	(const This& r)				{ x /= r.x; y /= r.y; z /= r.z; w /= r.w; }

	AX_INLINE constexpr bool operator == (const This& r) const			{ return x == r.x && y == r.y && z == r.z && w == r.w; }
	AX_INLINE constexpr bool operator != (const This& r) const			{ return x != r.x || y != r.y || z != r.z || w != r.w; }

	AX_INLINE constexpr bool equals		(const This& r, T ep = ax_epsilon<T>()) const;

//	AX_INLINE This operator *  (const Mat4& m) const;

	template<class V> AX_INLINE constexpr 
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE constexpr 
	static This s_cast(const V& v) { return This(static_cast<T>(v.x), 
														static_cast<T>(v.y),
														static_cast<T>(v.z),
														static_cast<T>(v.w)); }
};

using axVec4i = axVec4< axInt >;
using axVec4f = axVec4< axFloat32 >;
using axVec4d = axVec4< axFloat64 >;

template<class T> AX_INLINE axVec4<T> operator+(T s, const axVec4<T>& v) { return axVec4<T>(s + v._v); }
template<class T> AX_INLINE axVec4<T> operator-(T s, const axVec4<T>& v) { return axVec4<T>(s - v._v); }
template<class T> AX_INLINE axVec4<T> operator*(T s, const axVec4<T>& v) { return axVec4<T>(s * v._v); }
template<class T> AX_INLINE axVec4<T> operator/(T s, const axVec4<T>& v) { return axVec4<T>(s / v._v); }

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axVec4<T> > {
		const static bool value = isTriviallyCopyable<T>::value;
	};
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axVec4<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axVec4<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axVec4<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axVec4<T>& value) { se.reader.readValue(value); }

// 1 arg
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec4<T> FUNC(const axVec4<T>& a) { \
		return axVec4<T>( \
			FUNC(a.x), \
			FUNC(a.y), \
			FUNC(a.z), \
			FUNC(a.w)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_floor)
	AX_MACRO_OP(ax_ceil)
	AX_MACRO_OP(ax_round)
	AX_MACRO_OP(ax_abs)
	AX_MACRO_OP(ax_clamp01)
	AX_MACRO_OP(ax_degrees)
	AX_MACRO_OP(ax_radius)
#undef AX_MACRO_OP

// 2 args
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec4<T> FUNC(const axVec4<T>& a, const axVec4<T>& b) { \
		return axVec4<T>( \
			FUNC(a.x, b.x), \
			FUNC(a.y, b.y), \
			FUNC(a.z, b.z), \
			FUNC(a.w, b.w)  \
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
	axVec4<T> FUNC(const axVec4<T>& a, const axVec4<T>& b, const axVec4<T>& c) { \
		return axVec4<T>( \
			FUNC(a.x, b.x, c.x), \
			FUNC(a.y, b.y, c.y), \
			FUNC(a.z, b.z, c.z), \
			FUNC(a.w, b.w, c.w)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_clamp)
#undef AX_MACRO_OP

// to_int
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec4i	FUNC(const axVec4<T>& v) { \
		return axVec4i(FUNC(v.x), FUNC(v.y), FUNC(v.z), FUNC(v.w)); \
	} \
//-----
	AX_MACRO_OP(ax_trunc_to_int)
	AX_MACRO_OP(ax_round_to_int)
	AX_MACRO_OP(ax_ceil_to_int)
	AX_MACRO_OP(ax_floor_to_int)
#undef AX_MACRO_OP


template<class T> constexpr 
axVec4<T> ax_lerp(const axVec4<T> & a, const axVec4<T> & b, const T& weight) {
	return axVec4<T>(
		ax_lerp(a.x, b.x, weight),
		ax_lerp(a.y, b.y, weight),
		ax_lerp(a.z, b.z, weight),
		ax_lerp(a.w, b.w, weight)
	);
}

constexpr 
axVec4<axInt> ax_lerp(const axVec4<axInt> & a, const axVec4<axInt> & b, float weight) {
	return axVec4<axInt>(
		(axInt)ax_lerp(a.x, b.x, weight),
		(axInt)ax_lerp(a.y, b.y, weight),
		(axInt)ax_lerp(a.z, b.z, weight),
		(axInt)ax_lerp(a.w, b.w, weight)
	);
}

template<class T> constexpr  
bool axVec4<T>::equals(const This& r, T ep) const
{
	return ax_math_equals(x, r.x, ep)
		&& ax_math_equals(y, r.y, ep)
		&& ax_math_equals(z, r.z, ep)
		&& ax_math_equals(w, r.w, ep);
}

template<class T> AX_INLINE constexpr 	axVec4<T> axVec2<T>::xy01() const { return Vec4(x,y,0,1); }
template<class T> AX_INLINE constexpr	axVec4<T> axVec3<T>::xyz1() const { return Vec4(x,y,z,1); }

template<class T> AX_INLINE constexpr void axTuple4<T>::operator=(const Vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
