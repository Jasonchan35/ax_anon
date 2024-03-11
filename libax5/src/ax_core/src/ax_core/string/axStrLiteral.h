#pragma once

#include "axStrView.h"

template< class T >
class axStrLiteralT : public axStrMutViewT<const T> {
	using This = axStrLiteralT<T>;
	using Base = axStrMutViewT<const T>;
public:
	using View = axStrMutViewT<const T>;

	AX_INLINE constexpr axStrLiteralT() : Base(axCStr::kEmpty<T>(), 0) {}

	template<axInt N>
	AX_INLINE constexpr axStrLiteralT(const T (&sz)[N]) : Base(sz, N ? N-1 : 0) {}

	View asStrView() const { return *static_cast<const View*>(this); }

// axStrLiteral will always null terminated
	AX_INLINE constexpr const T*	c_str	() const { return Base::data(); }
};

namespace axTypeTraits {
	template<class T>	struct spanType<    axStrLiteralT<T> > { using Type = axStrViewT<T>; };
	template<class T>	struct mutSpanType< axStrLiteralT<T> > { using Type = axStrMutViewT<T>; };
}

AX_INLINE axStrLiteral axStrLiteral_bool(bool v) { return v ? axStrLiteral("true") : axStrLiteral("false"); }
