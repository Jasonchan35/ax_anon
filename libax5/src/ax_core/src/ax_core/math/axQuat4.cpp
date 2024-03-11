#include "axQuat4.h"
#include <ax_core/rtti/axTypeManager.h>

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axQuat4<T>)

#define	AX_MACRO_OP(T)	\
	template class axQuat4<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
