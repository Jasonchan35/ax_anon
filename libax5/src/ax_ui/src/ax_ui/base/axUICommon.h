#pragma once

#include <ax_ui/ax_ui_config.h>

class axUIView;
class axUIWindow;
class axRenderContext;

#define axUILayoutMode_EnumList(E) \
	E(Expand,)		\
	E(FitContent,)	\
	E(Fixed,)		\
//----
ax_ENUM_CLASS(axUILayoutMode, axUInt8);

#define axUIAlignX_EnumList(E) \
	E(Left,)	\
	E(Center,)	\
	E(Right,)	\
//----
ax_ENUM_CLASS(axUIAlignX, axUInt8);

#define axUIAlignY_EnumList(E) \
	E(Top,)		\
	E(Center,)	\
	E(Bottom,)	\
//----
ax_ENUM_CLASS(axUIAlignY, axUInt8);

class axUIAlign {
public:
	using X = axUIAlignX;
	using Y = axUIAlignY;

	axUIAlign() = default;
	constexpr axUIAlign(X x_, Y y_) : x(x_), y(y_) {}

	static constexpr axUIAlign s_topLeft		()	{ return {X::Left,		Y::Top}; }
	static constexpr axUIAlign s_topCenter		()	{ return {X::Center,	Y::Top}; }
	static constexpr axUIAlign s_topRight		()	{ return {X::Right,		Y::Top}; }

	static constexpr axUIAlign s_centerLeft		()	{ return {X::Left,		Y::Center}; }
	static constexpr axUIAlign s_center			()	{ return {X::Center,	Y::Center}; }
	static constexpr axUIAlign s_centerRight	()	{ return {X::Right,		Y::Center}; }

	static constexpr axUIAlign s_bottomLeft		()	{ return {X::Left,		Y::Bottom}; }
	static constexpr axUIAlign s_bottomCenter	()	{ return {X::Center,	Y::Bottom}; }
	static constexpr axUIAlign s_bottomRight	()	{ return {X::Right,		Y::Bottom}; }

	axVec2f posInRect(const axRect2f& rect, const axVec2f& contentSize) const;

	axUIAlignX	x = X::Left;
	axUIAlignY	y = Y::Top;
};

class axUIMeasured {
	using This = axUIMeasured;
public:
	axVec2f		min;
	axVec2f		max;

	axUIMeasured() { reset(); }

	void reset() {
		min.set(0,0);
		max.set(ax_inf, ax_inf);
	}

	void onFormat(axFormat& f) const;

	bool operator==(const This& r) const { return min == r.min && max == r.max; }
	bool operator!=(const This& r) const { return min != r.min || max != r.max; }
};


class axUIMeasureRequest {
public:
	using Result = axUIMeasured;
	Result result;
};
