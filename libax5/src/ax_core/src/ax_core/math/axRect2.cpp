#include "axRect2.h"
#include "../rtti/axTypeManager.h"

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axRect2<T>)

#define	AX_MACRO_OP(T)	\
	template class axRect2<T>; \
//-------------
	AX_MACRO_OP(axInt)
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
