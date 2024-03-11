#pragma once

#include "../base/ax_math.h"
#include "../rtti/axType.h"
#include "../string/axFormat.h"
#include "../data_format/json/axJsonSerializer.h"
#include "axFloat16.h"
#include "axUNorm.h"

template<class T> class axVec2;
template<class T> class axVec3;
template<class T> class axVec4;
template<class T> class axTuple2;
template<class T> class axTuple3;
template<class T> class axTuple4;
template<class T> class axMat4;
template<class T> class axQuat4;
template<class T> class axRect2;

//! no alignment required (therefore no SIMD), but good for vertex data
template<class T>
class axTuple2 {
	using This = axTuple2;
public:
	using Element = T;
	static const axInt kElementCount = 2;

	using Vec2   = axVec2<T>;
	using Vec3   = axVec3<T>;
	using Vec4   = axVec4<T>;
	using Tuple2 = axTuple2<T>;
	using Tuple3 = axTuple3<T>;
	using Tuple4 = axTuple4<T>;
	using Rect2  = axRect2<T>;
	using Mat4   = axMat4<T>;
	using Quat4  = axQuat4<T>;

	T x, y;

	AX_INLINE explicit constexpr axTuple2() = default;
	AX_INLINE explicit constexpr axTuple2(const T& v) : x(v), y(v) {}
	AX_INLINE explicit constexpr axTuple2(T x_, T y_) : x(x_), y(y_) {}
	AX_INLINE explicit constexpr axTuple2(const Vec2& v) { operator=(v); }

	AX_INLINE constexpr void set(T x_, T y_) { x = x_; y = y_; }
	AX_INLINE constexpr void operator=(const Vec2& v);

	bool operator==(const This& r) const { return x == r.x && y == r.y; }
	bool operator!=(const This& r) const { return x != r.x || y != r.y; }

	void onFormat(axFormat& f) const {
		f << "[" << x << ", " << y << "]";
	}

	template<class V> AX_INLINE
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE
	static This s_cast(const V& v) { return This(static_cast<Element>(v.x), 
														static_cast<Element>(v.y)); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axTuple2<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple2<T>) == sizeof(T) * 2);
		const static bool value = isTriviallyCopyable<T>::value;
	};
	template<class T> struct isTriviallyComparable< axTuple2<T> > {
		AX_STATIC_ASSERT(sizeof(axTuple2<T>) == sizeof(T) * 2);
		const static bool value = isTriviallyComparable<T>::value;
	};
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axTuple2<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axTuple2<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axTuple2<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axTuple2<T>& value) { se.reader.readValue(value); }


using axInt8x2  = axTuple2< axInt8   >;
using axInt16x2 = axTuple2< axInt16  >;
using axInt32x2 = axTuple2< axInt32  >;
using axInt64x2 = axTuple2< axInt64  >;

using axUInt8x2  = axTuple2< axUInt8   >;
using axUInt16x2 = axTuple2< axUInt16  >;
using axUInt32x2 = axTuple2< axUInt32  >;
using axUInt64x2 = axTuple2< axUInt64  >;

using axUNorm8x2  = axTuple2< axUNorm8  >;
using axUNorm16x2 = axTuple2< axUNorm16 >;
using axUNorm32x2 = axTuple2< axUNorm32 >;

using axFloat16x2 = axTuple2< axFloat16 >;
using axFloat32x2 = axTuple2< axFloat32 >;
using axFloat64x2 = axTuple2< axFloat64 >;


