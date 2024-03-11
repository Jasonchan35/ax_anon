#pragma once

#include "../rtti/axTypeManager.h"
#include "../data_format/json/axJsonUtil.h"

class axFormat;

class axFloat16 {
public:
	using hdata = uint16_t;

	AX_STRUCT_BEGIN(axFloat16, axNoBase)
	AX_STRUCT_END()

	axFloat16() = default;
	explicit axFloat16(float f) { set(f); }
	void operator=(float f) { set(f); }

	float  toFloat32() const;
	double toFloat64() const { return toFloat32(); }

	void set(float  v);
	void set(double v) { set(static_cast<float>(v)); }

	static axFloat16 s_make(float  v) { axFloat16 o; o.set(v); return o; }
	static axFloat16 s_make(double v) { axFloat16 o; o.set(v); return o; }

	void onFormat(axFormat& f) const;

	bool operator==(const axFloat16 & r) const { return _v == r._v; }
	bool operator!=(const axFloat16 & r) const { return _v != r._v; }

	axFloat16 operator+(const axFloat16& r) const { return s_make(toFloat32() + r.toFloat32()); }
	axFloat16 operator-(const axFloat16& r) const { return s_make(toFloat32() - r.toFloat32()); }
	axFloat16 operator*(const axFloat16& r) const { return s_make(toFloat32() * r.toFloat32()); }
	axFloat16 operator/(const axFloat16& r) const { return s_make(toFloat32() / r.toFloat32()); }

	void  operator+=(const axFloat16& r) { *this = *this + r; }
	void  operator-=(const axFloat16& r) { *this = *this - r; }
	void  operator*=(const axFloat16& r) { *this = *this * r; }
	void  operator/=(const axFloat16& r) { *this = *this / r; }

	void setToDefaultValue() { set(0.0f); }

	hdata value() const { return _v; }
private:
	hdata _v;

	union uif32 {
		float	 f;
		uint32_t i;
	};

//	explicit axFloat16(hdata v) : _v(v) {};

	static float overflow() {
		volatile float f = 1e10;
		for(int i = 0; i < 10; ++i)	
			f *= f; // this will overflow before the for loop terminates
		return f;
	}
};

namespace axTypeTraits {
	template<> struct isTriviallyCopyable<axFloat16> { const static bool value = true; };
}

template<> const axType* ax_typeof<axFloat16>();

AX_INLINE axFloat16 axFloat16_make(float  v) { return axFloat16::s_make(v); }
AX_INLINE axFloat16 axFloat16_make(double v) { return axFloat16::s_make(v); }

bool axStrView_tryParse(axStrView s, axFloat16& value);

template<> inline
void axJsonReader_handler<axFloat16>(axJsonReader& json_, axFloat16& value) {
	float tmp;
	json_.readValue(tmp);
	value = tmp;
}

template<> inline
void axJsonWriter_handler<axFloat16>(axJsonWriter& json_, const axFloat16& value) {
	float tmp = value.toFloat32();
	json_.writeValue(tmp);
}

AX_STATIC_ASSERT(axTypeTraits::isPod<axFloat16>::value);

inline
void axFloat16::set(float f) {
	uif32 Entry;
	Entry.f = f;
	int i = (int)Entry.i;

	//
	// Our floating point number, f, is represented by the bit
	// pattern in integer i.  Disassemble that bit pattern into
	// the sign, s, the exponent, e, and the significand, m.
	// Shift s into the position where it will go in in the
	// resulting half number.
	// Adjust e, accounting for the different exponent bias
	// of float and half (127 versus 15).
	//

	int s =  (i >> 16) & 0x00008000;
	int e = ((i >> 23) & 0x000000ff) - (127 - 15);
	int m =   i        & 0x007fffff;

	//
	// Now reassemble s, e and m into a half:
	//

	if(e <= 0) {
		if(e < -10) {
			//
			// E is less than -10.  The absolute value of f is
			// less than half_MIN (f may be a small normalized
			// float, a denormalized float or a zero).
			//
			// We convert f to a half zero.
			//
			_v = hdata(s);
			return;
		}
		//
		// E is between -10 and 0.  F is a normalized float,
		// whose magnitude is less than __half_NRM_MIN.
		//
		// We convert f to a denormalized half.
		// 

		m = (m | 0x00800000) >> (1 - e);

		// Round to nearest, round "0.5" up.
		//
		// Rounding may cause the significand to overflow and make
		// our number normalized.  Because of the way a half's bits
		// are laid out, we don't have to treat this case separately;
		// the code below will handle it correctly.

		if(m & 0x00001000) 
			m += 0x00002000;

		// Assemble the half from s, e (zero) and m.
		_v = hdata(s | (m >> 13));
		return;

	} else if(e == 0xff - (127 - 15)) {
		if(m == 0) {
			// F is an infinity; convert f to a half
			// infinity with the same sign as f.
			_v = hdata(s | 0x7c00);
			return;
		} else {
			// F is a NAN; we produce a half NAN that preserves
			// the sign bit and the 10 leftmost bits of the
			// significand of f, with one exception: If the 10
			// leftmost bits are all zero, the NAN would turn 
			// into an infinity, so we have to set at least one
			// bit in the significand.
			m >>= 13;
			_v = hdata(s | 0x7c00 | m | (m == 0));
			return;
		}
	}
	else
	{
		// E is greater than zero.  F is a normalized float.
		// We try to convert f to a normalized half.
		//
		// Round to nearest, round "0.5" up
		if(m &  0x00001000) {
			m += 0x00002000;
			if(m & 0x00800000) {
				m =  0;     // overflow in significand,
				e += 1;     // adjust exponent
			}
		}

		// Handle exponent overflow
		if (e > 30) {
			overflow();        // Cause a hardware floating point overflow;

			_v = hdata(s | 0x7c00);
			return;
			// if this returns, the half becomes an
		}   // infinity with the same sign as f.

		//
		// Assemble the half from s, e and m.
		//

		_v = hdata(s | (e << 10) | (m >> 13));
		return;
	}
}

inline
float axFloat16::toFloat32() const {
		int s = (_v >> 15) & 0x00000001;
		int e = (_v >> 10) & 0x0000001f;
		int m =  _v        & 0x000003ff;

		if(e == 0) {
			if (m == 0) {
				// Plus or minus zero
				uif32 result;
				result.i = (unsigned int)(s << 31);
				return result.f;
			} else {
				// Denormalized number -- renormalize it
				while(!(m & 0x00000400)) {
					m <<= 1;
					e -=  1;
				}
				e += 1;
				m &= ~0x00000400;
			}
		} else if(e == 31) {
			if (m == 0) {
				// Positive or negative infinity
				uif32 result;
				result.i = (unsigned int)((s << 31) | 0x7f800000);
				return result.f;
			} else {
				//
				// Nan -- preserve sign and significand bits
				//

				uif32 result;
				result.i = (unsigned int)((s << 31) | 0x7f800000 | (m << 13));
				return result.f;
			}
		}

		// Normalized number
		e = e + (127 - 15);
		m = m << 13;

		// Assemble s, e and m.
		uif32 Result;
		Result.i = (uint32_t)((s << 31) | (e << 23) | m);
		return Result.f;
}
