#pragma once

#include "../base/ax_math.h"
#include "../rtti/axType.h"

template<class T>
class axBBox1 {
public:
	using Element = T;

	T	min = kScalarMax();
	T	max = kScalarMin();

	AX_STRUCT_BEGIN(axBBox1<T>, axNoBase)
		ax_FIELDS(min, max)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

	axBBox1() = default;
	axBBox1(const T& min_, const T& max_) : min(min_), max(max_) {}
	void set(const T& min_, const T& max_) { min = min_; max = max_; }

	void include(T pt);
	void include(const axBBox1& r);

	This	unionWith(const This& r) const;

	T	size() const { return isValid() ? max - min : 0; }

	AX_INLINE void clear() { *this = This(); };

	static T	kScalarMin();
	static T	kScalarMax();

	bool isValid() const { return min <= max; }
	explicit operator bool() const { return isValid(); }
};

template<class T> inline
void axBBox1<T>::include(T pt) {
	ax_min_it(min, pt);
	ax_max_it(max, pt);
}

template<class T> inline
void axBBox1<T>::include(const axBBox1& r) {
	ax_min_it(min, r.min);
	ax_max_it(max, r.max);
}

template<class T>
inline axBBox1<T> axBBox1<T>::unionWith(const This& r) const {
	if (!isValid()) return r;
	if (!r.isValid()) return *this;
	return This(ax_min(min, r.min), ax_max(max, r.max));
}

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axBBox1<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<> AX_INLINE axInt     axBBox1<axInt    >::kScalarMin() { return axNumLimit<axInt>::min(); }
template<> AX_INLINE axInt     axBBox1<axInt    >::kScalarMax() { return axNumLimit<axInt>::max(); }
template<> AX_INLINE axFloat32 axBBox1<axFloat32>::kScalarMin() { return -ax_inf_<axFloat32>(); }
template<> AX_INLINE axFloat32 axBBox1<axFloat32>::kScalarMax() { return  ax_inf_<axFloat32>(); }
template<> AX_INLINE axFloat64 axBBox1<axFloat64>::kScalarMin() { return -ax_inf_<axFloat64>(); }
template<> AX_INLINE axFloat64 axBBox1<axFloat64>::kScalarMax() { return  ax_inf_<axFloat64>(); }

using axBBox1i = axBBox1< axInt >;
using axBBox1f = axBBox1< axFloat32 >;
using axBBox1d = axBBox1< axFloat64 >;


