#pragma once
#include "axVec4.h"
#include "axMargin2.h"

template<class T>
class axRect2 {
public:
	typedef T Element;
	static const axInt kElementCount = 4;

	using Vec2   = axVec2<T>;
	using Vec3   = axVec3<T>;
	using Vec4   = axVec4<T>;
	using Margin2 = axMargin2<T>;

	union {
		struct { T x, y, w, h;};
		struct { Vec2 pos, size; };
		T data[kElementCount];
	};

	AX_STRUCT_BEGIN(axRect2<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(x, y, w, h)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	explicit	constexpr axRect2		() = default;
	explicit	constexpr axRect2		(T v) : x(v), y(v), w(v), h(v) {}
				constexpr axRect2		(T x_, T y_, T w_, T h_) : x(x_), y(y_), w(w_), h(h_) {}
				constexpr axRect2		(const Vec2& pos_, const Vec2& size_) : pos(pos_), size(size_) {}

	constexpr void	set			(T x_, T y_, T w_, T h_)				{ x = x_; y = y_; w = w_; h = h_; }
	constexpr void	set			(const Vec2& pos_, const Vec2& size_)	{ pos = pos_; size = size_; }
	constexpr void	setToDefaultValue() { set(0,0,0,0); }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(&x, kElementCount); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(&x, kElementCount); }

	constexpr Vec2		center		() const { return pos + size / 2; }
	constexpr T			xCenter		() const { return x + w / 2; }
	constexpr T			yCenter		() const { return y + h / 2; }

	constexpr T			xMin		() const { return x; }
	constexpr T			yMin		() const { return y; }
	constexpr T			xMax		() const { return x + w; }
	constexpr T			yMax		() const { return y + h; }

	constexpr Vec2		xMinYMin	() const { return Vec2(xMin(), yMin()); }
	constexpr Vec2		xMaxYMin	() const { return Vec2(xMax(), yMin()); }
	constexpr Vec2		xMinYMax	() const { return Vec2(xMin(), yMax()); }
	constexpr Vec2		xMaxYMax	() const { return Vec2(xMax(), yMax()); }

	constexpr Vec2		topLeft		() const { return xMinYMin(); }
	constexpr Vec2		topRight	() const { return xMaxYMin(); }
	constexpr Vec2		bottomLeft	() const { return xMinYMax(); }
	constexpr Vec2		bottomRight	() const { return xMaxYMax(); }

	constexpr void		setXMin		(T v)	{ auto m = xMax(); x = v; setXMax(m); }
	constexpr void		setXMax		(T v)	{ w = v - x; }
	constexpr void		setYMin		(T v)	{ auto m = yMax(); y = v; setYMax(m); }
	constexpr void		setYMax		(T v)	{ h = v - y; }

	constexpr void		setXMinYMin	(const Vec2& v) { setXMin(v.x); setYMin(v.y); }
	constexpr void		setXMaxYMin	(const Vec2& v) { setXMax(v.x); setYMin(v.y); }
	constexpr void		setXMinYMax	(const Vec2& v) { setXMin(v.x); setYMax(v.y); }
	constexpr void		setXMaxYMax	(const Vec2& v) { setXMax(v.x); setYMax(v.y); }

	AX_NODISCARD constexpr This		expand		(const Margin2& m) const					{ return axRect2(x - m.left, y - m.top, w + m.left + m.right, h + m.top + m.bottom ); }

	AX_NODISCARD constexpr This		expandTop	(T v) const			{ return expand({v,0,0,0}); }
	AX_NODISCARD constexpr This		expandRight	(T v) const			{ return expand({0,v,0,0}); }
	AX_NODISCARD constexpr This		expandBottom(T v) const			{ return expand({0,0,v,0}); }
	AX_NODISCARD constexpr This		expandLeft	(T v) const			{ return expand({0,0,0,v}); }

				constexpr This		trimTop		(T v) { v = ax_min(v, h); This o(x, y,          w, v); y += v; h -= v; return o; }
				constexpr This		trimLeft	(T v) { v = ax_min(v, w); This o(x, y,          v, h); x += v; w -= v; return o; }
				constexpr This		trimBottom	(T v) { v = ax_min(v, h); This o(x, yMax() - v, w, v); h -= v;         return o; }
				constexpr This		trimRight	(T v) { v = ax_min(v, w); This o(xMax() - v, y, v, h); w -= v;         return o; }

	AX_NODISCARD constexpr This		flipXWithinContainer	(T containerWidth ) const { return This(containerWidth - xMax(),  y, w, h); }
	AX_NODISCARD constexpr This		flipYWithinContainer	(T containerHeight) const { return This(x, containerHeight - yMax(), w, h); }

	AX_NODISCARD constexpr T			area		() const { return w * h; }
	AX_NODISCARD constexpr T			perimeter	() const { return w + w + h + h; }

	AX_NODISCARD constexpr This		offset		(const Vec2& v) const				{ return axRect2(pos + v, size); }

	AX_NODISCARD constexpr This		operator+	(const Vec2& v) const				{ return offset( v); }
	AX_NODISCARD constexpr This		operator-	(const Vec2& v) const				{ return offset(-v); }

	AX_NODISCARD constexpr Margin2	diff		(const axRect2& inner) const;
	AX_NODISCARD constexpr Margin2	operator-	(const axRect2& inner) const		{ return diff(inner); }

	AX_NODISCARD constexpr This		operator+	(const Margin2& m) const			{ return expand( m); }
	AX_NODISCARD constexpr This		operator-	(const Margin2& m) const			{ return expand(-m); }

				 constexpr void		operator+=	(const Margin2& m)					{ *this = expand( m); }
				 constexpr void		operator-=	(const Margin2& m)					{ *this = expand(-m); }

	AX_NODISCARD constexpr This		operator*	(T s) const							{ return This(x * s,   y * s,   w * s,   h * s); }
	AX_NODISCARD constexpr This		operator/	(T s) const							{ return This(x / s,   y / s,   w / s,   h / s); }

	AX_NODISCARD constexpr This		operator*	(const Vec2& s) const				{ return This(x * s.x, y * s.y, w * s.x, h * s.y); }
	AX_NODISCARD constexpr This		operator/	(const Vec2& s) const				{ return This(x / s.x, y / s.y, w / s.x, h / s.y); }

				 constexpr void		operator*=	(T s)								{ x *= s;   y *= s;   w *= s;   h *= s; }
				 constexpr void		operator/=	(T s)								{ x /= s;   y /= s;   w /= s;   h /= s; }

				 constexpr void		operator*=	(const Vec2& s)						{ x *= s.x; y *= s.y; w *= s.x; h *= s.y; }
				 constexpr void		operator/=	(const Vec2& s)						{ x /= s.x; y /= s.y; w /= s.x; h /= s.y; }

				 constexpr bool		operator==	(const This& r) const				{ return pos == r.pos && size == r.size; }
				 constexpr bool		operator!=	(const This& r) const				{ return pos != r.pos || size != r.size; }

				 constexpr This		unionWith		(const axRect2& r) const;
				 constexpr This		intersects		(const axRect2& r) const;
				 constexpr bool		isIntersected	(const axRect2& r) const;
	
	AX_NODISCARD constexpr bool		containsPoint(const Vec2& v) const	{ return v.x >= xMin() && v.x <= xMax() && v.y >= yMin() && v.y <= yMax(); }

	AX_NODISCARD constexpr Vec4		toVec4() const { return Vec4(x,y,w,h); }

	template<class V> AX_INLINE constexpr 
	static axRect2 s_cast(const V& v) { return axRect2(	static_cast<Element>(v.x), 
																static_cast<Element>(v.y),
																static_cast<Element>(v.w),
																static_cast<Element>(v.h)); }

#if AX_USE_Win32Api
	explicit constexpr axRect2(const RECT& r) { set(r); }

	inline constexpr void set( const RECT &src ) {
		set((T)src.left, (T)src.top, (T)(src.right - src.left), (T)(src.bottom - src.top) );
	}

	inline constexpr RECT to_RECT() const {
		RECT o;
		o.left = static_cast<LONG>(xMin());	o.right  = static_cast<LONG>(xMax());
		o.top  = static_cast<LONG>(yMin());	o.bottom = static_cast<LONG>(yMax());
		return o;
	}
#endif

#if AX_OS_MACOSX || AX_OS_IOS
	// CGRect is NSRect in MacOSX AppKit
	AX_INLINE explicit axRect2(const CGRect& r) { set(r); }

	AX_INLINE void set( const CGRect &r ) { pos.set(r.origin); size.set(r.size); }

	AX_INLINE CGRect to_CGRect() const {
		return CGRect{{static_cast<float>(x), static_cast<float>(y)},
					  {static_cast<float>(w), static_cast<float>(h)}};
	}
	AX_INLINE operator CGRect() const { return to_CGRect(); }
#endif
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axRect2<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline void axJsonReader_handler(axJsonReader& rd,       axRect2<T>& value) { rd.readValue( value.span()); }
template<class T> inline void axJsonWriter_handler(axJsonWriter& wr, const axRect2<T>& value) { wr.writeValue(value.span()); }

template<class T> inline void axJsonSerializer_handler	(axJsonSerializer&   se, axRect2<T>& value) { se.writer.writeValue(value); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axRect2<T>& value) { se.reader.readValue(value); }

typedef axRect2<axInt>		axRect2i;
typedef axRect2<axFloat32>	axRect2f;
typedef axRect2<axFloat64>	axRect2d;

template<class T> inline constexpr 
axMargin2<T> axRect2<T>::diff(const axRect2 & inner) const {
	return axMargin2<T>(inner.yMin() - yMin(),
						xMax() - inner.xMax(),
						yMax() - inner.yMax(),
						inner.xMin() - xMin());
}

#if AX_OS_MACOSX || AX_OS_IOS
AX_INLINE void axFormatParam_handler(axFormat& f, const CGRect & v) {
	return axFormatParam_handler(f, axRect2f(v));
}
#endif

template<class T> inline constexpr 
bool axRect2<T>::isIntersected(const axRect2& r) const {
	if (r.x > xMax() || x > r.xMax()) return false;
	if (r.y > yMax() || y > r.yMax()) return false;
	return true;
}

template<class T> inline constexpr 
axRect2<T> axRect2<T>::unionWith(const axRect2& r) const {
	Vec2 topLeft	(ax_min(xMin(), r.xMin()),
					 ax_min(yMin(), r.yMin()));
	Vec2 bottomRight(ax_max(xMax(), r.xMax()),
					 ax_max(yMax(), r.yMax()));

	return This(topLeft, bottomRight - topLeft);
}

template<class T> inline constexpr 
axRect2<T> axRect2<T>::intersects(const axRect2& r) const {
	if (!isIntersected(r)) return This(0,0,0,0);

	Vec2 topLeft	(ax_max(xMin(), r.xMin()),
					 ax_max(yMin(), r.yMin()));
	Vec2 bottomRight(ax_min(xMax(), r.xMax()),
					 ax_min(yMax(), r.yMax()));

	return This(topLeft, bottomRight - topLeft);
}
