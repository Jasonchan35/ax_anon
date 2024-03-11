#include "axBBox1.h"
#include <ax_core/rtti/axTypeManager.h>

AX_REFLECTION_IO_FUNC_IMPL(	template<class T>, axBBox1<T>)

#define	AX_MACRO_OP(T) \
	template class axBBox1<T>; \
//-------------
	AX_MACRO_OP(axFloat32)
	AX_MACRO_OP(axFloat64)
#undef AX_MACRO_OP
