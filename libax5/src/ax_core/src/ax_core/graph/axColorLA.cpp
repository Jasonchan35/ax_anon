#include "axColorLA.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axColorLA_<T>)

#define	AX_MACRO_OP(T, VALUE, SUFFIX)	\
	template class axColorLA_<T>; \
//-------------
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP
