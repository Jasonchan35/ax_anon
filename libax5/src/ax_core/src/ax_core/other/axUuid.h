//
//  axUUID.h
//  ax_core
//
//  Created by Jason on 2013-11-14.
//
//

#pragma once

#include "../string/axFormat.h"

#if AX_COMPILER_VC
	#pragma comment( lib, "Rpcrt4.lib" )
#endif

//copyable
class axUuid {
public:
	static constexpr axInt kDataSize = 16;

	struct Value {
		union {
			axUInt8 data[kDataSize] = {0};
			struct {
				axUInt64 q0; // quad word
				axUInt64 q1;
			};
			struct {
				axUInt32 e0;
				axUInt16 e1;
				axUInt16 e2;
				axUInt64 e3;
			};
		};
	};

	AX_STATIC_ASSERT(sizeof(Value) == kDataSize);

	constexpr void setToZero() { value.q0 = 0; value.q1 = 0; }

	static axUuid s_zero() { return axUuid(); }

	void generate();
	static axUuid s_generate() { axUuid t; t.generate(); return t; }

	void onFormat(axFormat& f) const;
	bool tryParse(axStrView str);
	void parse(axStrView str) { if (!tryParse(str)) throw axError_Undefined(AX_LOC); }

	template<class SE>
	void onSerialize(SE& se) { AX_SE_IO(se, value.q0, value.q1); }

	bool operator==(const axUuid& r) const { return value.q0 == r.value.q0 && value.q1 == r.value.q1; }
	bool operator!=(const axUuid& r) const { return value.q0 != r.value.q0 || value.q1 != r.value.q1; }

	constexpr bool isValid() const { return value.q0 != 0 && value.q1 != 0; }
	constexpr explicit operator bool() const { return isValid(); }

	axHash onGetHash() const { return axHash(value.q0 ^ value.q1); }

	AX_INLINE axUuid swapByteOrderForVariant2() const;

	Value value;
};

AX_STATIC_ASSERT(sizeof(axUuid) == axUuid::kDataSize);

// Variant 2 UUIDs, historically used in Microsoft's COM/OLE libraries, use a mixed-endian format, 
// whereby the first three components of the UUID are little-endian, and the last two are big-endian. 
// For example, 00112233-4455-6677-8899-aabbccddeeff is encoded 
// as the bytes 33 22 11 00 55 44 77 66 88 99 aa bb cc dd ee ff.

inline
axUuid axUuid::swapByteOrderForVariant2() const {
	axUuid o;
	using S = axByteOrder::ByteSwapUInt;
	o.value.e0 = S::convert(value.e0);
	o.value.e1 = S::convert(value.e1);
	o.value.e2 = S::convert(value.e2);
	o.value.e3 = value.e3;
	return o;
}
