#pragma once

#include "../string/axFormat.h"

template<class T> class axUNormT;
using axUNorm8  = axUNormT<axUInt8>;
using axUNorm16 = axUNormT<axUInt16>;
using axUNorm32 = axUNormT<axUInt32>;

namespace axTypeTraits {
	template<> struct makeFloat<axUNorm8>  { using Type = axFloat32; };
	template<> struct makeFloat<axUNorm16> { using Type = axFloat32; };
	template<> struct makeFloat<axUNorm32> { using Type = axFloat64; };
}

template<class T> struct axUNormBase;
template<> struct axUNormBase<axUInt8> {
	using Int    = axUInt8;
	using BigInt = axUInt64;
	static const Int kIntMin  = 0;
	static const Int kIntMax  = UINT8_MAX;
};

template<> struct axUNormBase<axUInt16> {
	using Int    = axUInt16;
	using BigInt = axUInt64;
	static const Int kIntMin  = 0;
	static const Int kIntMax  = UINT16_MAX;
};

template<> struct axUNormBase<axUInt32> {
	using Int    = axUInt32;
	using BigInt = axUInt64;
	static const Int kIntMin  = 0;
	static const Int kIntMax  = UINT32_MAX;
};

// unsigned normalized value
template<class T>
class axUNormT : public axUNormBase<T> {
public:
	using UNormBase = axUNormBase<T>;

	AX_STRUCT_BEGIN(axUNormT<T>, axNoBase)
	AX_STRUCT_END()

	template<class SE>
	void onSerialize(SE& se) {
		AX_SE_IO(se, value);
	}

	using Float = typename axTypeTraits::makeFloat<T>::Type;
	using Int   = T;
	using UNormBase::kIntMin;
	using UNormBase::kIntMax;
	using typename UNormBase::BigInt;

	static const axUNormT kMin()  { return axUNormT(kIntMin); }
	static const axUNormT kMax()  { return axUNormT(kIntMax); }
	static const axUNormT kHalf() { return axUNormT(kIntMax / 2); }

	axUNormT() = default;
	constexpr axUNormT(Int v) : value(v) {}

	//axUNormT(float v)		{ *this = s_make(v); }
	//void operator=(float v) { *this = s_make(v); }

	Float toFloat() const { return static_cast<Float>(value) / kIntMax; }
	void setFloat(Float f) { *this = s_fromFloat(f); }

	static constexpr This s_fromFloat(Float f) { 
		return axUNormT(static_cast<Int>(ax_clamp(f * kIntMax, static_cast<Float>(kIntMin), static_cast<Float>(kIntMax))));
	}

	constexpr bool operator==(const This &r) const { return value == r.value; }
	constexpr bool operator!=(const This &r) const { return value != r.value; }
	constexpr bool operator> (const This &r) const { return value >  r.value; }
	constexpr bool operator>=(const This &r) const { return value >= r.value; }
	constexpr bool operator< (const This &r) const { return value <  r.value; }
	constexpr bool operator<=(const This &r) const { return value <= r.value; }

	constexpr This operator+(const This& r) const { return axUNormT(value + r.value); }
	constexpr This operator-(const This& r) const { return axUNormT(value - r.value); }
	constexpr This operator*(const This& r) const {
		return axUNormT(static_cast<T>( static_cast<BigInt>(value) * static_cast<BigInt>(r.value) / kIntMax));
	}
	constexpr This operator/(const This& r) const {
		return axUNormT(static_cast<T>((static_cast<BigInt>(value) * kIntMax) / (static_cast<BigInt>(r.value) * kIntMax)));
	}

	constexpr void operator+=(const This& r) { value += r.value; }
	constexpr void operator-=(const This& r) { value -= r.value; }

	constexpr void operator*=(const This& r) { *this = *this * r; }
	constexpr void operator/=(const This& r) { *this = *this / r; }

	void onFormat(axFormat& f) const { f << value; }
	void onJsonReader(axJsonReader& rd)			{ rd.readValue(value);  }
	void onJsonWriter(axJsonWriter& wr) const	{ wr.writeValue(value); }

	constexpr void setToDefaultValue() { value = 0; }

	T	value;
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axUNormT<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T, class W> AX_INLINE
axUNormT<T>	ax_lerp(const axUNormT<T>& a, const axUNormT<T>& b, W w) {
	return ax_lerp(a.value, b.value, w);
}