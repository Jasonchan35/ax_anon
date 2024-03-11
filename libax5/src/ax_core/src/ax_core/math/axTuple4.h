#pragma once

#include "axTuple3.h"

//! no alignment required (therefore no SIMD), but good for vertex data
template<class T>
class axTuple4 {
	using This = axTuple4;
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

	AX_INLINE explicit constexpr axTuple4() = default;
	AX_INLINE explicit constexpr axTuple4(const T& v) : x(v), y(v), z(v), w(v) {}
	AX_INLINE explicit constexpr axTuple4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
	AX_INLINE explicit constexpr axTuple4(const Vec4& v) { operator=(v); }

	AX_INLINE constexpr axTuple2<T>	xy() const	{ return axTuple2<T>(x,y); }
	AX_INLINE constexpr axTuple2<T>	xz() const	{ return axTuple2<T>(x,z); }
	AX_INLINE constexpr axTuple2<T>	yz() const	{ return axTuple2<T>(y,z); }
	AX_INLINE constexpr axTuple3<T>	xyz() const	{ return axTuple3<T>(x,y,z); }

	AX_INLINE constexpr void set(T x_, T y_, T z_, T w_)		{ x = x_;  y = y_;  z = z_;  w = w_;  }
	AX_INLINE constexpr void operator=(const Vec4& v);

	bool operator==(const This& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	bool operator!=(const This& r) const { return x != r.x || y != r.y || z != r.z || w != r.w; }

	void onFormat(axFormat& f) const {
		f << "[" << x << ", " << y << ", " << z << ", " << w << "]";
	}

	template<class V> AX_INLINE
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE
	static This s_cast(const V& v) { return This(static_cast<Element>(v.x), 
														static_cast<Element>(v.y),
														static_cast<Element>(v.z),
														static_cast<Element>(v.w)); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axTuple4<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple4<T>) == sizeof(T) * 4);
		const static bool value = isTriviallyCopyable<T>::value;
	};
	template<class T> struct isTriviallyComparable< axTuple4<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple4<T>) == sizeof(T) * 4);
		const static bool value = isTriviallyComparable<T>::value;
	};
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axTuple4<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axTuple4<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axTuple4<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axTuple4<T>& value) { se.reader.readValue(value); }

using axInt8x4  = axTuple4< axInt8   >;
using axInt16x4 = axTuple4< axInt16  >;
using axInt32x4 = axTuple4< axInt32  >;
using axInt64x4 = axTuple4< axInt64  >;

using axUInt8x4  = axTuple4< axUInt8   >;
using axUInt16x4 = axTuple4< axUInt16  >;
using axUInt32x4 = axTuple4< axUInt32  >;
using axUInt64x4 = axTuple4< axUInt64  >;

using axUNorm8x4  = axTuple4< axUNorm8  >;
using axUNorm16x4 = axTuple4< axUNorm16 >;
using axUNorm32x4 = axTuple4< axUNorm32 >;

using axFloat16x4 = axTuple4< axFloat16 >;
using axFloat32x4 = axTuple4< axFloat32 >;
using axFloat64x4 = axTuple4< axFloat64 >;
