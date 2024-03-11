#pragma once

#include "ax_basic_types.h"

class axDebugEnumerateCounterError : public axError {};

template<class T = axInt>
class axDebugEnumerateCounterT {
public:
	axDebugEnumerateCounterT()
		: _count(0) 
	{};

	AX_INLINE	T	operator++	() 	{ return ++_count; }
	AX_INLINE	T	operator--	()	{ return --_count; }

	void checkZero() {
		if (_count != 0) {
			throw axDebugEnumerateCounterError();
		}
	}

private:
	T _count;
};

typedef axDebugEnumerateCounterT<> axDebugEnumerateCounter;

