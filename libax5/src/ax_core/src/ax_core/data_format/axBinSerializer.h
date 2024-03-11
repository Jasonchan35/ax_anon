#pragma once

#include "axBinDeserializer.h"

class axBinSerializer : public axNonCopyable {
public:
	using LittleEndian = axByteOrder::LittleEndian;

	axBinSerializer(axIByteArray& buf)
		: _buf(&buf)
	{}

	void io(bool&       value)	{ axByte tmp = value ? 1 : 0; _io_fixed(tmp); }

	void io(axInt8&      value) { _io_vary_sint(value); }
	void io(axInt16&     value) { _io_vary_sint(value); }
	void io(axInt32&     value) { _io_vary_sint(value); }
	void io(axInt64&     value) { _io_vary_sint(value); }

	void io(axUInt8&     value) { _io_vary_uint(value); }
	void io(axUInt16&    value) { _io_vary_uint(value); }
	void io(axUInt32&    value) { _io_vary_uint(value); }
	void io(axUInt64&    value) { _io_vary_uint(value); }

	void io_fixed(axInt8&     value) { _io_fixed(value); }
	void io_fixed(axInt16&    value) { _io_fixed(value); }
	void io_fixed(axInt32&    value) { _io_fixed(value); }
	void io_fixed(axInt64&    value) { _io_fixed(value); }

	void io_fixed(axUInt8&    value) { _io_fixed(value); }
	void io_fixed(axUInt16&   value) { _io_fixed(value); }
	void io_fixed(axUInt32&   value) { _io_fixed(value); }
	void io_fixed(axUInt64&   value) { _io_fixed(value); }

	void io(axFloat32&  value) { _io_fixed(value); }
	void io(axFloat64&  value) { _io_fixed(value); }

	void io(axStrView8  value) {
		auto n = value.size();
		io(n);
		io_raw(value.byteSpan());
	}

	template<class T>
	void io(axStrViewT<T> value) {
		auto tmp = axTempString8::s_convert(value);
		io(tmp);
	}

	template<class T>
	void io(axMutSpan<T> value) {
		auto n = value.size();
		io(n);
		for (auto& e : value) {
			io(e);
		}
	}

	template<class T>			void io(axIArray<T>&  value) { io<T>(value.span()); }
	template<class T, axInt N>	void io(axArray<T,N>& value) { io<T>(value.span()); }

	template<class T> void io(T& value) { axBinSerializer_handler(*this, value); }

	//template<class T> void named_io(axStrView name, T& value) { io(value); }
	//template<class T> void named_io_fixed(axStrView name, T& value) { io_fixed(value); }
	//template<class T> void named_io_pkey(axStrView name, T& value, bool autoPkeypkeyAutoInc) { named_io(name, value); }

	void io_raw(axByteSpan span) {
		_buf->appendRange(span);
	}
private:

// https://sqlite.org/src4/doc/trunk/www/varint.wiki
//  Variable-Length Integers - from SQLite4
//  
//  A variable length integer is an encoding of 64-bit unsigned integers into between 1 and 9 bytes. The encoding has the following properties:
//  
//  1) Smaller (and more common) values use fewer bytes and take up less space than larger (and less common) values.
//  2) The length of any varint can be determined by looking at just the first byte of the encoding.
//  3) Lexicographical and numeric ordering for varints are the same. 
//     Hence if a group of varints are order lexicographically (that is to say, if they are order by memcmp() with shorter varints coming first) 
//     then those varints will also be in numeric order. This property means that varints can be used as keys in the key/value backend storage
//     and the records will occur in numerical order of the keys.
//  4) The encoding is described by algorithms to decode (convert from varint to 8-byte unsigned integer) 
//     and to encode (convert from 8-byte unsigned integer to varint). Treat each byte of the encoding as an unsigned integer between 0 and 255. 
//     Let the bytes of the encoding be called A0, A1, A2, ..., A8.
//  
//  == Decode ==
//  If A0 is between 0 and 240 inclusive, then the result is the value of A0.
//  If A0 is between 241 and 248 inclusive, then the result is 240+256*(A0-241)+A1.
//  If A0 is 249 then the result is 2288+256*A1+A2.
//  If A0 is 250 then the result is A1..A3 as a 3-byte big-ending integer.
//  If A0 is 251 then the result is A1..A4 as a 4-byte big-ending integer.
//  If A0 is 252 then the result is A1..A5 as a 5-byte big-ending integer.
//  If A0 is 253 then the result is A1..A6 as a 6-byte big-ending integer.
//  If A0 is 254 then the result is A1..A7 as a 7-byte big-ending integer.
//  If A0 is 255 then the result is A1..A8 as a 8-byte big-ending integer.
//  
//  == Encode ==
//  Let the input value be V.
//  If V<=240 then output a single by A0 equal to V.
//  If V<=2287 then output A0 as (V-240)/256 + 241 and A1 as (V-240)%256.
//  If V<=67823 then output A0 as 249, A1 as (V-2288)/256, and A2 as (V-2288)%256.
//  If V<=16777215 then output A0 as 250 and A1 through A3 as a big-endian 3-byte integer.
//  If V<=4294967295 then output A0 as 251 and A1..A4 as a big-ending 4-byte integer.
//  If V<=1099511627775 then output A0 as 252 and A1..A5 as a big-ending 5-byte integer.
//  If V<=281474976710655 then output A0 as 253 and A1..A6 as a big-ending 6-byte integer.
//  If V<=72057594037927935 then output A0 as 254 and A1..A7 as a big-ending 7-byte integer.
//  Otherwise then output A0 as 255 and A1..A8 as a big-ending 8-byte integer.
//  
//  == Other information ==
//  Bytes | MaxValue | Digits
//  1     | 240      | 2.3
//  2     | 2287     | 3.3
//  3     | 67823    | 4.8
//  4     | 2^24-1   | 7.2
//  5     | 2^32-1   | 9.6
//  6     | 2^40-1   | 12.0
//  7     | 2^48-1   | 14.4
//  8     | 2^56-1   | 16.8
//  9     | 2^64-1   | 19.2

	template<class T> AX_INLINE
	void _io_vary_uint_write1(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(1);
		auto* p = _buf->data() + oldSize;
		*p = static_cast<axByte>(value);
	}

	template<class T> AX_INLINE
	void _io_vary_uint_write2(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(2);
		auto* p = _buf->data() + oldSize;
		auto tmp = value - 240U;
		p[0] = static_cast<axByte>(tmp / 256U + 241U);
		p[1] = static_cast<axByte>(tmp % 256U);
	}

	template<class T> AX_INLINE
	void _io_vary_uint_write3(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(3);
		auto* p = _buf->data() + oldSize;
		auto tmp = value - 2288U;
		p[0] = 249U;
		p[1] = static_cast<axByte>(tmp / 256U);
		p[2] = static_cast<axByte>(tmp % 256U);
	}

	template<class T> AX_INLINE
	void _io_vary_uint_write4(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(4);
		auto* p = _buf->data() + oldSize;
		p[0] = 250;
		_writeBytes(p+1, static_cast<axUInt16>(value));
		p[3] = static_cast<axByte>(value >> 16);
	}

	template<class T>
	void _io_vary_uint_write5(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(5);
		auto* p = _buf->data() + oldSize;
		p[0] = 251;
		_writeBytes(p+1, static_cast<axUInt32>(value));
	}

	template<class T>
	void _io_vary_uint_write6(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(6);
		auto* p = _buf->data() + oldSize;
		p[0] = 252;
		_writeBytes(p+1, static_cast<axUInt32>(value));
		p[5] = static_cast<axByte>(value >> 32);
	}

	template<class T>
	void _io_vary_uint_write7(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(7);
		auto* p = _buf->data() + oldSize;
		p[0] = 253;
		_writeBytes(p+1, static_cast<axUInt32>(value));
		p[5] = static_cast<axByte>(value >> 32);
		p[6] = static_cast<axByte>(value >> 40);
	}

	template<class T>
	void _io_vary_uint_write8(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(8);
		auto* p = _buf->data() + oldSize;
		p[0] = 254;
		_writeBytes(p+1, static_cast<axUInt32>(value));
		p[5] = static_cast<axByte>(value >> (8*4));
		p[6] = static_cast<axByte>(value >> (8*5));
		p[7] = static_cast<axByte>(value >> (8*6));
	}

	template<class T>
	void _io_vary_uint_write9(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		auto oldSize = _buf->size();
		_buf->incSize(9);
		auto* p = _buf->data() + oldSize;
		p[0] = 255;
		_writeBytes(p+1, static_cast<axUInt64>(value));
	}

	AX_INLINE void _io_vary_uint(axUInt8& value) {
		if (value <= 240                 ) return _io_vary_uint_write1(value);
		return _io_vary_uint_write2(value);
	}

	AX_INLINE void _io_vary_uint(axUInt16& value) {
		if (value <= 240                 ) return _io_vary_uint_write1(value);
		if (value <= 2287                ) return _io_vary_uint_write2(value);
		return _io_vary_uint_write3(value);
	}

	AX_INLINE void _io_vary_uint(axUInt32& value) {
		if (value <= 240UL               ) return _io_vary_uint_write1(value);
		if (value <= 2287UL              ) return _io_vary_uint_write2(value);
		if (value <= 67823UL             ) return _io_vary_uint_write3(value);
		if (value <= 16777215UL          ) return _io_vary_uint_write4(value);
		return _io_vary_uint_write5(value);
	}

	AX_INLINE void _io_vary_uint(axUInt64& value) {
		if (value <= 240ULL              ) return _io_vary_uint_write1(value);
		if (value <= 2287ULL             ) return _io_vary_uint_write2(value);
		if (value <= 67823ULL            ) return _io_vary_uint_write3(value);
		if (value <= 16777215ULL         ) return _io_vary_uint_write4(value);
		if (value <= 4294967295ULL       ) return _io_vary_uint_write5(value);
		if (value <= 1099511627775ULL    ) return _io_vary_uint_write6(value);
		if (value <= 281474976710655ULL  ) return _io_vary_uint_write7(value);
		if (value <= 72057594037927935ULL) return _io_vary_uint_write8(value);
		return _io_vary_uint_write9(value);
	}

//------- io vary signed ---------
//  Encode signed to unsigned int using ZigZag (wrap the first bit to the end)
//  to keep the most significant bit (MSB) small after static_cast to unsigned
//
//  == Example ==
//  int | encoded uint
//   0  | 0
//  -1  | 1
//   1  | 2
//  -2  | 3
//   2  | 4
//  -3  | 5
//   3  | 6
 
	template<class T>
	void _io_vary_sint(T& value) {
		using U = typename axTypeTraits::makeUnsigned<T>::Type;
		const axInt kBitCount = ax_sizeof(T) * 8;
		U tmp = static_cast<U>( (value << 1) ^ (value >> (kBitCount - 1)) );
		_io_vary_uint(tmp);
	}

//----- Fixed ---------
	template<class T>
	void _io_fixed(T& value) {
		auto oldSize = _buf->size();
		_buf->incSize(ax_sizeof(T));
		auto* dst = _buf->data() + oldSize;
		using U = typename axTypeTraits::typeBySize<sizeof(T)>::UIntType;
		_writeBytes(dst, *reinterpret_cast<U*>(&value));
	}

	AX_INLINE void _writeBytes(axByte* dst, axUInt8 value) {
		dst[0] = static_cast<axByte>(value);
	}

	AX_INLINE void _writeBytes(axByte* dst, axUInt16 value) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 2
		*reinterpret_cast<axUInt16*>(dst) = LittleEndian::fromHost(value);
	#else
		dst[0] = static_cast<axByte>(value);
		dst[1] = static_cast<axByte>(value >> (8*1));
	#endif
	}

	AX_INLINE void _writeBytes(axByte* dst, axUInt32 value) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 4
		*reinterpret_cast<axUInt32*>(dst) = LittleEndian::fromHost(value);
	#else
		dst[0] = static_cast<axByte>(value);
		dst[1] = static_cast<axByte>(value >> (8*1));
		dst[2] = static_cast<axByte>(value >> (8*2));
		dst[3] = static_cast<axByte>(value >> (8*3));
	#endif
	}

	AX_INLINE void _writeBytes(axByte* dst, axUInt64 value) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 8
		*reinterpret_cast<axUInt64*>(dst) = LittleEndian::fromHost(value);
	#else
		dst[0] = static_cast<axByte>(value);
		dst[1] = static_cast<axByte>(value >> (8*1));
		dst[2] = static_cast<axByte>(value >> (8*2));
		dst[3] = static_cast<axByte>(value >> (8*3));
		dst[4] = static_cast<axByte>(value >> (8*4));
		dst[5] = static_cast<axByte>(value >> (8*5));
		dst[6] = static_cast<axByte>(value >> (8*6));
		dst[7] = static_cast<axByte>(value >> (8*7));
	#endif
	}

	axIByteArray* _buf = nullptr;
};

template<class T> inline
void axBinSerializer_handler(axBinSerializer& se, axIStringT<T>& value) {
	se.io(value.view());
}

template<class T, axInt N> inline
void axBinSerializer_handler(axBinSerializer& se, axStringT<T, N>& value) {
	se.io(value.view());
}

template<class CLOCK> AX_INLINE
void axBinSerializer_handler(axBinSerializer& se, axTimestampT<CLOCK>& value) {
	auto t = value.toInt();
	se.io(t);
}

template<class T> AX_INLINE
void axBinSerializer_handler(axBinSerializer& se, T& value) {
	value.onSerialize(se);
}
