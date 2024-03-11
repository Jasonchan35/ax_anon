#include "axColorHSBA.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axColorHSBA_<T>)

#define	AX_MACRO_OP(T, VALUE, SUFFIX)	\
	template class axColorHSBA_<T>; \
//-------------
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP
