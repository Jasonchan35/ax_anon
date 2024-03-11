//
//  safe_assign.h
//  ax_core
//
//  Created by Jason on 2015-03-15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

#include "ax_basic_types.h"
#include "ax_utils.h"

template<class DST, class SRC> bool	ax_try_safe_assign( DST& dst, const SRC& src );

//same type assign
#define	AX_MACRO_OP(T) \
	template<> inline bool ax_try_safe_assign( T& dst, const T& src ) { dst = src; return true; } \
//-----
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
	AX_TYPE_LIST_EXTRA_NATIVE_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_EXTRA_NATIVE_UINT(AX_MACRO_OP)
#undef AX_MACRO_OP

template<class DST, class SRC> inline
bool ax_try_safe_assign( DST& dst, const SRC& src ) {
	DST tmp = static_cast<DST>(src);

	//unsigned << signed
	if( axTypeTraits::isUnsigned<DST>::value && axTypeTraits::isSigned<SRC>::value ) {
		if( ax_less_than0( src ) ) {
			return false;
		}
	}

	// signed << unsigned
	if( axTypeTraits::isSigned<DST>::value && axTypeTraits::isUnsigned<SRC>::value ) {
		if( ax_less_than0( tmp ) ) {
			return false;
		}
	}

	//avoid overflow
	if( src != static_cast<SRC>(tmp) ) {
		return false;
	}

	dst = tmp; //done
	return true;
}

template<class DST, class SRC> inline
void ax_assert_safe_assign(DST& dst, const SRC& src) {
	if (!ax_try_safe_assign(dst, src)) {
		AX_ASSERT(false);
		dst = static_cast<DST>(src); // still assign
	}
}

template<class DST, class SRC> inline
void ax_safe_assign(DST& dst, const SRC& src) {
	if (!ax_try_safe_assign(dst, src)) {
		throw axError_SafeAssign();
	}
}

template<class DST, class SRC> inline
DST ax_safe_static_cast(const SRC& src) {
	DST o;
	ax_safe_assign(o, src);
	return o;
}