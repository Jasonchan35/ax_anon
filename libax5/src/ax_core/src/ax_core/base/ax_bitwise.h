//
//  ax_bitwise.h
//  axon_core
//
//  Created by Jason on 2014-12-15.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "ax_basic_types.h"
#include "ax_utils.h"

class ax_bitwise {
public:
	template<class T>		constexpr	static bool	hasAny	(T  value, T bits)			{ return (value & bits) != T(0);    }
	template<class T>		constexpr	static bool	has		(T  value, T bits)			{ return (value & bits) == bits; }	
	template<class T>	AX_INLINE	static void		set		(T& value, T bits, bool b)	{ b ? set(value, bits) : unset(value, bits); }
	template<class T>	AX_INLINE	static void		set		(T& value, T bits)			{ value |=  bits; }
	template<class T>	AX_INLINE	static void		unset	(T& value, T bits)			{ value &= ~bits; }
	template<class T>	AX_INLINE	static void		toggle	(T& value, T bits)			{ value ^=  bits; }
	template<class T>	AX_INLINE	static axInt	count1	(T  value) {
		axInt result = 0;
		axInt n = ax_sizeof(T) * 8;
		T m = T(1);
		for (axInt i = 0; i < n; i++) {
			if (hasAny(value, m)) result++;
			m <<= 1;
		}
		return result;
	}
	template<class T>	AX_INLINE	static axInt	count0	(T  value) { return count1(~value); }
	template<class T>	AX_INLINE	static axInt	highest(T  value) {
		axInt result = -1;
		axInt n = ax_sizeof(T) * 8;
		T m = T(1);
		for (axInt i = 0; i < n; i++) {
			if (hasAny(value, m)) result = i;
			m <<= 1;
		}
		return result;
	}
	template<class T>	AX_INLINE	static axInt	lowest(T  value) {
		axInt result = -1;
		axInt n = ax_sizeof(T) * 8;
		T m = T(1 << (n-1));
		for (axInt i = n-1; i > 0; i--) {
			if (hasAny(value, m)) result = i;
			m >>= 1;
		}
		return result;
	}
};
