#include "axUNorm.h"

#define	AX_MACRO_OP(T)	\
	template class axUNormT<T>; \
//-------------
	AX_MACRO_OP(axUInt8)
	AX_MACRO_OP(axUInt16)
	AX_MACRO_OP(axUInt32)
#undef AX_MACRO_OP
