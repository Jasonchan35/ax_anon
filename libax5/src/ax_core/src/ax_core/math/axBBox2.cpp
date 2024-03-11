#include "axBBox2.h"
#include <ax_core/rtti/axTypeManager.h>

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axBBox2<T>)

#define	AX_MACRO_OP(T) \
	template class axBBox2<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
