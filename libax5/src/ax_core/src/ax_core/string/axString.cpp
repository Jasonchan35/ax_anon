#include "axString.h"

#define	AX_MACRO_OP(T)	\
	/*---- The explicit instantiation ---*/ \
	template class axStringT<T>; \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef	AX_MACRO_OP
