#pragma once

#include "../string/axFormat.h"

template<class T>
class axMargin2 {
public:
	using Vec2    = axVec2<T>;
	using Margin2 = axMargin2;

	typedef T Element;
	static const axInt kElementCount = 4;

	union {
		struct { T top, right, bottom, left;}; //as same as CSS Margin order
		T data[kElementCount];
	};	

	AX_STRUCT_BEGIN(axMargin2<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(top, right, bottom, left)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	explicit axMargin2() = default;
			 axMargin2(T v)										{ set(v); }
			 axMargin2(T x, T y)								{ set(y, x, y, x); }
			 axMargin2(T top_, T right_, T bottom_, T left_)	{ set(top_, right_, bottom_, left_); }
	
	void set(T v)									{ top = v; right = v; bottom = v; left = v; }
	void set(T x, T y)								{ top = y; right = x; bottom = y; left = x; }
	void set(T top_, T right_, T bottom_, T left_)	{ top = top_; right = right_; bottom = bottom_; left = left_; }

	AX_NODISCARD This inflate(T x, T y) const { return This( top - y, right + x, bottom + y, left - x); }
	AX_NODISCARD This operator-() const { return This(-top, -right, -bottom, -left); }

	AX_NODISCARD Vec2	total	() const { return axVec2<T>(totalX(), totalY()); }
	AX_NODISCARD T		totalX	() const { return left + right;  }
	AX_NODISCARD T		totalY	() const { return top  + bottom; }

	AX_NODISCARD Vec2	topLeft		() const { return Vec2(left,  top   ); }
	AX_NODISCARD Vec2	topRight	() const { return Vec2(right, top   ); }
	AX_NODISCARD Vec2	bottomLeft	() const { return Vec2(left,  bottom); }
	AX_NODISCARD Vec2	bottomRight	() const { return Vec2(right, bottom); }

	bool		operator==	(const This& r) const	{ return top == r.top && right == r.right && bottom == r.bottom && left == r.left; }
	bool		operator!=	(const This& r) const	{ return top != r.top || right != r.right || bottom != r.bottom || left != r.left; }
	
	Margin2		operator+	(const T& r) const { return Margin2(top + r, right + r, bottom + r, left + r); }
	Margin2		operator-	(const T& r) const { return Margin2(top - r, right - r, bottom - r, left - r); }
	Margin2		operator*	(const T& r) const { return Margin2(top * r, right * r, bottom * r, left * r); }
	Margin2		operator/	(const T& r) const { return Margin2(top / r, right / r, bottom / r, left / r); }

	Margin2		operator+	(const Vec2& r) const { return Margin2(top + r.y, right + r.x, bottom + r.y, left + r.x); }
	Margin2		operator-	(const Vec2& r) const { return Margin2(top - r.y, right - r.x, bottom - r.y, left - r.x); }
	Margin2		operator*	(const Vec2& r) const { return Margin2(top * r.y, right * r.x, bottom * r.y, left * r.x); }
	Margin2		operator/	(const Vec2& r) const { return Margin2(top / r.y, right / r.x, bottom / r.y, left / r.x); }

	Margin2		operator+	(const Margin2& r) const { return Margin2(top + r.top, right + r.right, bottom + r.bottom, left + r.left); }
	Margin2		operator-	(const Margin2& r) const { return Margin2(top - r.top, right - r.right, bottom - r.bottom, left - r.left); }
	Margin2		operator*	(const Margin2& r) const { return Margin2(top * r.top, right * r.right, bottom * r.bottom, left * r.left); }
	Margin2		operator/	(const Margin2& r) const { return Margin2(top / r.top, right / r.right, bottom / r.bottom, left / r.left); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axMargin2<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axMargin2<T>)

using axMargin2i = axMargin2<axInt>;
using axMargin2f = axMargin2<axFloat32>;
