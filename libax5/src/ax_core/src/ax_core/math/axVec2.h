#pragma once

#include "axTuple4.h"

template<class T>
class axVec2 {
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
	AX_STRUCT_BEGIN(axVec2<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(x, y);
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();
	
	AX_INLINE explicit constexpr axVec2() = default;
	AX_INLINE explicit constexpr axVec2(T v) : x(v), y(v) {}
	AX_INLINE			constexpr axVec2(T x_, T y_) : x(x_), y(y_) {}
	AX_INLINE			constexpr axVec2(const Tuple2& t)	{ operator=(t); }

	AX_INLINE		 T* data()			{ return &x; }
	AX_INLINE const T* data() const	{ return &x; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(&x, kElementCount); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(&x, kElementCount); }

	AX_INLINE constexpr Vec2 xy		() const { return Vec2(x, y); }
	AX_INLINE constexpr Vec2 yx		() const { return Vec2(y, x); }

	AX_INLINE constexpr Vec3	xy0		() const; // impl in Vec3
	AX_INLINE constexpr Vec3	xy1		() const; // impl in Vec3
	AX_INLINE constexpr Vec3	x0y		() const; // impl in Vec3
	AX_INLINE constexpr Vec3	x1y		() const; // impl in Vec3
	AX_INLINE constexpr Vec4	xy01	() const; // impl in Vec4

	AX_INLINE constexpr void set(T x_, T y_)				{ x = x_;  y = y_; }
	AX_INLINE constexpr void setToDefaultValue()			{ set(0,0); }

	AX_INLINE constexpr void operator=(const Tuple2& t)	{ set(t.x, t.y); }

	AX_INLINE constexpr This	operator - () const					{ return This(-x, -y); }

	AX_INLINE constexpr This operator + (T s) const				{ return This(x + s, y + s); }
	AX_INLINE constexpr This operator - (T s) const				{ return This(x - s, y - s); }
	AX_INLINE constexpr This operator * (T s) const				{ return This(x * s, y * s); }
	AX_INLINE constexpr This operator / (T s) const				{ return This(x / s, y / s); }

	AX_INLINE constexpr This operator + (const This& r) const		{ return This(x + r.x, y + r.y); }
	AX_INLINE constexpr This operator - (const This& r) const		{ return This(x - r.x, y - r.y); }
	AX_INLINE constexpr This operator * (const This& r) const		{ return This(x * r.x, y * r.y); }
	AX_INLINE constexpr This operator / (const This& r) const		{ return This(x / r.x, y / r.y); }

	AX_INLINE constexpr void operator += (T s)						{ x += s; y += s; }
	AX_INLINE constexpr void operator -= (T s)						{ x -= s; y -= s; }
	AX_INLINE constexpr void operator *= (T s)						{ x *= s; y *= s; }
	AX_INLINE constexpr void operator /= (T s)						{ x /= s; y /= s; }

	AX_INLINE constexpr void operator += (const This& r)			{ x += r.x; y += r.y; }
	AX_INLINE constexpr void operator -= (const This& r)			{ x -= r.x; y -= r.y; }
	AX_INLINE constexpr void operator *= (const This& r)			{ x *= r.x; y *= r.y; }
	AX_INLINE constexpr void operator /= (const This& r)			{ x /= r.x; y /= r.y; }

	AX_INLINE constexpr bool operator == (const This& r) const		{ return x == r.x && y == r.y; }
	AX_INLINE constexpr bool operator != (const This& r) const		{ return x != r.x || y != r.y; }

	AX_INLINE constexpr T	lengthSq() const						{ return x * x + y * y; }
	AX_INLINE constexpr T	length () const							{ return ax_sqrt(lengthSq()); }
	AX_INLINE constexpr bool	lengthLessThan(T v)					{ return lengthSq() < (v * v); }
	AX_INLINE constexpr bool	lengthGreaterThan(T v)				{ return lengthSq() > (v * v); }

	AX_INLINE constexpr T	distance	(const This &r) const		{ return (*this - r).length(); }
	AX_INLINE constexpr T	distanceSq	(const This &r) const		{ return (*this - r).lengthSq(); }

	AX_INLINE constexpr bool equals	(const Vec3& r, T ep = ax_epsilon<T>()) const;
	AX_INLINE constexpr bool equals0	(T ep = ax_epsilon<T>()) const;

#if AX_USE_Win32Api
	AX_INLINE constexpr explicit axVec2( const POINT &src )	{ set(src); }
	AX_INLINE constexpr void set( const POINT &src )			{ set( static_cast<T>(src.x),     static_cast<T>(src.y) ); }

#endif //AX_OS_WINDOWS

#if AX_OS_MACOSX || AX_OS_IOS
	// CGPoint is NSPoint in MacOSX AppKit
	
	AX_INLINE explicit constexpr axVec2( const CGPoint &src )	{ set(src); }
	AX_INLINE explicit constexpr axVec2( const CGSize  &src )	{ set(src); }

	AX_INLINE constexpr void set( const CGPoint &src )			{ set( static_cast<T>(src.x),     static_cast<T>(src.y) ); }
	AX_INLINE constexpr void set( const CGSize  &src )			{ set( static_cast<T>(src.width), static_cast<T>(src.height)); }

	AX_INLINE constexpr CGPoint to_CGPoint() const				{ return CGPoint({static_cast<float>(x), static_cast<float>(y)}); }
	AX_INLINE constexpr CGSize  to_CGSize () const				{ return CGSize ({static_cast<float>(x), static_cast<float>(y)}); }
	
	AX_INLINE constexpr operator CGPoint() const				{ return to_CGPoint(); }
	AX_INLINE constexpr operator CGSize () const				{ return to_CGSize(); }
	
#endif //AX_OS_MACOSX || AX_OS_IOS

	template<class V> AX_INLINE constexpr 
	void setByCast(const V& v) { *this = s_cast(v); }

	template<class V> AX_INLINE constexpr 
	static axVec2 s_cast(const V& v) { return axVec2(	static_cast<T>(v.x), 
														static_cast<T>(v.y)); }
};

using axVec2i = axVec2< axInt >;
using axVec2f = axVec2< axFloat32 >;
using axVec2d = axVec2< axFloat64 >;

template<class T> AX_INLINE constexpr axVec2<T> operator+(T s, const axVec2<T>& v) { return axVec2<T>(s + v.x, s + v.y); }
template<class T> AX_INLINE constexpr axVec2<T> operator-(T s, const axVec2<T>& v) { return axVec2<T>(s - v.x, s - v.y); }
template<class T> AX_INLINE constexpr axVec2<T> operator*(T s, const axVec2<T>& v) { return axVec2<T>(s * v.x, s * v.y); }
template<class T> AX_INLINE constexpr axVec2<T> operator/(T s, const axVec2<T>& v) { return axVec2<T>(s / v.x, s / v.y); }

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axVec2<T> > { 
		const static bool value = isTriviallyCopyable<T>::value;
	};
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axVec2<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axVec2<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axVec2<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axVec2<T>& value) { se.reader.readValue(value); }

template<class T> AX_INLINE constexpr 
bool axVec2<T>::equals0(T ep) const {
	return ax_math_equals0(x, ep) 
		&& ax_math_equals0(y, ep);
}

template<class T> AX_INLINE constexpr 
bool axVec2<T>::equals(const Vec3& r, T ep) const {
	return ax_math_equals(x, r.x, ep)
		&& ax_math_equals(y, r.y, ep);
}

template<class T> AX_INLINE constexpr 
axVec2<T> ax_lerp(const axVec2<T> & a, const axVec2<T> & b, T weight) {
	return axVec3<T>(
		ax_lerp(a.x, b.x, weight),
		ax_lerp(a.y, b.y, weight)
	);
}

// 1 arg
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec2<T> FUNC(const axVec2<T>& a) { \
		return axVec2<T>( \
			FUNC(a.x), \
			FUNC(a.y)  \
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
	axVec2<T> FUNC(const axVec2<T>& a, const axVec2<T>& b) { \
		return axVec2<T>( \
			FUNC(a.x, b.x), \
			FUNC(a.y, b.y)  \
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
	axVec2<T> FUNC(const axVec2<T>& a, const axVec2<T>& b, const axVec2<T>& c) { \
		return axVec2<T>( \
			FUNC(a.x, b.x, c.x), \
			FUNC(a.y, b.y, c.y)  \
		); \
	} \
//---
	AX_MACRO_OP(ax_clamp)
#undef AX_MACRO_OP

// to_int
#define AX_MACRO_OP(FUNC) \
	template<class T> AX_INLINE constexpr \
	axVec2i	FUNC(const axVec2<T>& v) { \
		return axVec2i(FUNC(v.x), FUNC(v.y)); \
	} \
//-----
	AX_MACRO_OP(ax_trunc_to_int)
	AX_MACRO_OP(ax_round_to_int)
	AX_MACRO_OP(ax_ceil_to_int)
	AX_MACRO_OP(ax_floor_to_int)
#undef AX_MACRO_OP

template<class T> AX_INLINE constexpr void axTuple2<T>::operator=(const Vec2& v) { x = v.x; y = v.y; }

//----------------------------
#if AX_OS_MACOSX || AX_OS_IOS
AX_INLINE void axFormatParam_handler(axFormat& f, const CGPoint & v) {
	return axFormatParam_handler(f, axVec2f(v));
}
AX_INLINE void axFormatParam_handler(axFormat& f, const CGSize & v) {
	return axFormatParam_handler(f, axVec2f(v));
}
#endif

