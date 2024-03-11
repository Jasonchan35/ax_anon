#pragma once

#include "axTuple2.h"

//! no alignment required (therefore no SIMD), but good for vertex data
template<class T>
class axTuple3 {
	using This = axTuple3;
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

	AX_INLINE explicit constexpr axTuple3() = default;
	AX_INLINE explicit constexpr axTuple3(const T& v) : x(v), y(v), z(v) {}
	AX_INLINE explicit constexpr axTuple3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
	AX_INLINE explicit constexpr axTuple3(const Vec3& v) { operator=(v); }

	AX_INLINE constexpr axTuple2<T>	xy() const	{ return axTuple2<T>(x,y); }
	AX_INLINE constexpr axTuple2<T>	xz() const	{ return axTuple2<T>(x,z); }
	AX_INLINE constexpr axTuple2<T>	yz() const	{ return axTuple2<T>(y,z); }

	AX_INLINE constexpr void set(T x_, T y_, T z_)				{ x = x_; y = y_; z = z_; }
	AX_INLINE constexpr void operator=(const Vec3& v);

	AX_INLINE void operator+=(const Tuple3& r)			{ x += r.x; y += r.y; z += r.z; }

	bool operator==(const This& r) const { return x == r.x && y == r.y && z == r.z; }
	bool operator!=(const This& r) const { return x != r.x || y != r.y || z != r.z; }

	void onFormat(axFormat& f) const {
		f << "[" << x << ", " << y << ", " << z << "]";
	}

	template<class V> AX_INLINE
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE
	static This s_cast(const V& v) { return This(static_cast<Element>(v.x), 
														static_cast<Element>(v.y),
														static_cast<Element>(v.z)); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axTuple3<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple3<T>) == sizeof(T) * 3);
		const static bool value = isTriviallyCopyable<T>::value;
	};
	template<class T> struct isTriviallyComparable< axTuple3<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple3<T>) == sizeof(T) * 3);
		const static bool value = isTriviallyComparable<T>::value;
	};
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axTuple3<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axTuple3<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axTuple3<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axTuple3<T>& value) { se.reader.readValue(value); }

using axInt8x3  = axTuple3< axInt8   >;
using axInt16x3 = axTuple3< axInt16  >;
using axInt32x3 = axTuple3< axInt32  >;
using axInt64x3 = axTuple3< axInt64  >;

using axUInt8x3  = axTuple3< axUInt8   >;
using axUInt16x3 = axTuple3< axUInt16  >;
using axUInt32x3 = axTuple3< axUInt32  >;
using axUInt64x3 = axTuple3< axUInt64  >;

using axUNorm8x3  = axTuple3< axUNorm8  >;
using axUNorm16x3 = axTuple3< axUNorm16 >;
using axUNorm32x3 = axTuple3< axUNorm32 >;

using axFloat16x3 = axTuple3< axFloat16 >;
using axFloat32x3 = axTuple3< axFloat32 >;
using axFloat64x3 = axTuple3< axFloat64 >;
