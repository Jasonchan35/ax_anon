#pragma once

#include "axColorUtil.h"

using axColor = axColorRGBAf;

#define AX_MACRO_OP(T,...) \
	template<> struct axColorByType_< axColorType::T > { using Type = axColor ## T; };
//----
	axColorType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP

template<class COLOR>
class axColorPairT {
		using This = axColorPairT;
public:
	using Color = COLOR;

	axColorPairT() = default;
	axColorPairT(const Color& v) : c0(v), c1(v) {}
	axColorPairT(const Color& v0, const Color& v1) : c0(v0), c1(v1) {}

	COLOR c0, c1;
};

using axColorPair = axColorPairT<axColor>;