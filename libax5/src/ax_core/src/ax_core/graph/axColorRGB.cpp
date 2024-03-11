#include "axColorRGB.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axColorRGB_<T>)

template<class T>
void axColorRGB_<T>::toHexString(axIString& s) const {
	auto* hex = axCStr::kHexChars();
	s.clear();
	s.reserve(10);
	auto tmp = axColorRGBb_make(*this);

	s.append("#");
	s.append(hex[(tmp.r.value >> 4) & 0xf], hex[tmp.r.value & 0xf]);
	s.append(hex[(tmp.g.value >> 4) & 0xf], hex[tmp.g.value & 0xf]);
	s.append(hex[(tmp.b.value >> 4) & 0xf], hex[tmp.b.value & 0xf]);

}

#define	AX_MACRO_OP(T, VALUE, SUFFIX)	\
	template class axColorRGB_<T>; \
//-------------
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP
