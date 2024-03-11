#pragma once

#include "../container/axSpan.h"

namespace axMath {

template<class T> inline
T sum(axSpan<T> arr) {
	T o = 0;
	for (auto& v : arr) {
		o += v;
	}
	return o;
}

template<class T> inline
T average(axSpan<T> arr) {
	return sum(arr) / arr.size();
}

}