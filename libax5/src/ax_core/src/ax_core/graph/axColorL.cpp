#include "axColorL.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axColorL_<T>)

#define	AX_MACRO_OP(T, VALUE, SUFFIX)	\
	template class axColorL_<T>; \
//-------------
	axColorElementType_EnumList(AX_MACRO_OP)
#undef AX_MACRO_OP
