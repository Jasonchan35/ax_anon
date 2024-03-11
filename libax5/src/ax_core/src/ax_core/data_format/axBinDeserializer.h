#pragma once

#include <ax_core/string/axString.h>

class axBinDeserializer : public axNonCopyable {
public:
	using LittleEndian = axByteOrder::LittleEndian;

	axBinDeserializer(axByteSpan buf)
		: _buf(buf)
		, _remain(buf)
	{}

	void io(bool&       value)	{ axByte tmp; _io_fixed(tmp); value = (tmp != 0); }

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

	void io(axIString8&  value);
	void io(axIByteArray& value);

	template<class T> void io(axIArray<T>& value) {
		axInt len = 0;
		io(len);
		if (len > remain().size()) {
			// prevent insane len cause out of memory attack
			throw axError_Undefined(AX_LOC);
		}
		value.resize(len);
		for (auto& e : value) {
			io(e);
		}
	}

	template<class T, axInt N> void io(axArray<T,N>& value) { io(value.asIArray()); }

	template<class T> void io(T& value) { axBinDeserializer_handler(*this, value); }

	template<class T> void io_le(T& value) { T tmp; io_fixed(tmp); value = axByteOrder::LittleEndian::toHost(tmp); }
	template<class T> void io_be(T& value) { T tmp; io_fixed(tmp); value = axByteOrder::BigEndian::toHost(tmp); }

	template<class T> void named_io(axStrView name, T& value) { io(value); }
	template<class T> void named_io_fixed(axStrView name, T& value) { io_fixed(value); }
	template<class T> void named_io_pkey(axStrView name, T& value, bool autoPkeypkeyAutoInc) { named_io(name, value); }

	void io_raw(axByteMutSpan span) {
		ax_memcpy(span.data(), _remain.data(), span.sizeInBytes());
		_advance(span.sizeInBytes());
	}

	AX_INLINE void advance(axInt n) { _advance(n); }

	axByteSpan remain() const { return _remain; }

private:

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

	template<class T> AX_INLINE
	void _io_vary_uint_read1(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 1) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] <= 240U);
		value	= p[0];
		_advance(1);
	}

	template<class T> AX_INLINE
	void _io_vary_uint_read2(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 2) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] >= 241 && p[0] <= 248U);
		value	= static_cast<T>(240U + 256U * (p[0] - 241U) + p[1]);
		_advance(2);
	}

	template<class T> AX_INLINE
	void _io_vary_uint_read3(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 3) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 249);
		value	= static_cast<T>(2288U + 256U * p[1] + p[2]);
		_advance(3);
	}

	template<class T>
	void _io_vary_uint_read4(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 4) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 250U);
		axUInt16 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp)
		      | static_cast<T>(p[3]) << (8*2);
		_advance(4);
	}
	
	template<class T>
	void _io_vary_uint_read5(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 5) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 251U);
		axUInt32 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp);
		_advance(5);
	}
	
	template<class T>
	void _io_vary_uint_read6(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 6) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 252U);
		axUInt32 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp)
		      | static_cast<T>(p[5]) << (8*4);
		_advance(6);
	}
	
	template<class T>
	void _io_vary_uint_read7(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 7) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 253U);
		axUInt32 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp)
		      | static_cast<T>(p[5]) << (8*4)
		      | static_cast<T>(p[6]) << (8*5);
		_advance(7);
	}
	
	template<class T>
	void _io_vary_uint_read8(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 8) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 254U);
		axUInt32 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp)
		      | static_cast<T>(p[5]) << (8*4)
		      | static_cast<T>(p[6]) << (8*5)
		      | static_cast<T>(p[7]) << (8*6);
		_advance(8);
	}
	
	template<class T>
	void _io_vary_uint_read9(T& value) {
		AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<T>::value);
		if (_remain.size() < 9) {
			throw axError_Undefined(AX_LOC);
		}
		auto* p = _remain.data();
		AX_ASSERT(p[0] == 255U);
		axUInt64 tmp;
		_readBytes(tmp, p+1);
		value = static_cast<T>(tmp);
		_advance(9);
	}

	AX_INLINE
	void _io_vary_uint(axUInt8& value) {
		if (_remain.size() < 1) {
			throw axError_Undefined(AX_LOC);
		}
		auto first = *_remain.data();
		if (first <= 240U) return _io_vary_uint_read1(value);
		if (first <= 248U) return _io_vary_uint_read2(value);
		throw axError_Undefined(AX_LOC);
	}

	AX_INLINE
	void _io_vary_uint(axUInt16& value) {
		if (_remain.size() < 1) {
			throw axError_Undefined(AX_LOC);
		}
		auto first = *_remain.data();
		if (first <= 240U) return _io_vary_uint_read1(value);
		if (first <= 248U) return _io_vary_uint_read2(value);
		if (first == 249U) return _io_vary_uint_read3(value);
		throw axError_Undefined(AX_LOC);
	}

	AX_INLINE
	void _io_vary_uint(axUInt32& value) {
		if (_remain.size() < 1) {
			throw axError_Undefined(AX_LOC);
		}
		auto first = *_remain.data();
		if (first <= 240U) return _io_vary_uint_read1(value);
		if (first <= 248U) return _io_vary_uint_read2(value);
		if (first == 249U) return _io_vary_uint_read3(value);
		if (first == 250U) return _io_vary_uint_read4(value);
		if (first == 251U) return _io_vary_uint_read5(value);
		throw axError_Undefined(AX_LOC);
	}

	AX_INLINE
	void _io_vary_uint(axUInt64& value) {
		if (_remain.size() < 1) {
			throw axError_Undefined(AX_LOC);
		}
		auto first = *_remain.data();
		if (first <= 240U) return _io_vary_uint_read1(value);
		if (first <= 248U) return _io_vary_uint_read2(value);
		if (first == 249U) return _io_vary_uint_read3(value);
		if (first == 250U) return _io_vary_uint_read4(value);
		if (first == 251U) return _io_vary_uint_read5(value);
		if (first == 252U) return _io_vary_uint_read6(value);
		if (first == 253U) return _io_vary_uint_read7(value);
		if (first == 254U) return _io_vary_uint_read8(value);
		if (first == 255U) return _io_vary_uint_read9(value);
		throw axError_Undefined(AX_LOC);
	}

	template<class T> AX_INLINE
	void _io_vary_sint(T& value) {
		using U = typename axTypeTraits::makeUnsigned<T>::Type;
		U tmp;
		_io_vary_uint(tmp);
		value = static_cast<T>(tmp >> 1) ^ ( -static_cast<T>(tmp & 1) );
	}

	template<class T> AX_INLINE
	void _io_fixed(T& value) {
		if (_remain.size() < ax_sizeof(T)) {
			throw axError_Undefined(AX_LOC);
		}
		using U = typename axTypeTraits::typeBySize<sizeof(T)>::UIntType;
		U tmp;
		_readBytes(tmp, _remain.data());
		value = *reinterpret_cast<T*>(&tmp);
		_advance(ax_sizeof(T));
	}

	AX_INLINE void _readBytes(axUInt8& value, const axByte* src) {
		value  = static_cast<axUInt8>(src[0]);
	}

	AX_INLINE void _readBytes(axUInt16& value, const axByte* src) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 2
		value = LittleEndian::toHost(*reinterpret_cast<const axUInt16*>(src));
	#else
		value  = static_cast<axUInt16>(src[0]);
		value |= static_cast<axUInt16>(src[1]) << (8*1);
	#endif
	}

	AX_INLINE void _readBytes(axUInt32& value, const axByte* src) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 4
		value = LittleEndian::toHost(*reinterpret_cast<const axUInt32*>(src));
	#else
		value  = static_cast<axUInt32>(src[0]);
		value |= static_cast<axUInt32>(src[1]) << (8*1);
		value |= static_cast<axUInt32>(src[2]) << (8*2);
		value |= static_cast<axUInt32>(src[3]) << (8*3);
	#endif
	}

	AX_INLINE void _readBytes(axUInt64& value, const axByte* src) {
	#if AX_CPU_MISALIGNED_MEMORY_ACCESSES >= 8
		value = LittleEndian::toHost(*reinterpret_cast<const axUInt64*>(src));
	#else
		value  = static_cast<axUInt64>(src[0]);
		value |= static_cast<axUInt64>(src[1]) << (8*1);
		value |= static_cast<axUInt64>(src[2]) << (8*2);
		value |= static_cast<axUInt64>(src[3]) << (8*3);
		value |= static_cast<axUInt64>(src[4]) << (8*4);
		value |= static_cast<axUInt64>(src[5]) << (8*5);
		value |= static_cast<axUInt64>(src[6]) << (8*6);
		value |= static_cast<axUInt64>(src[7]) << (8*7);
	#endif
	}

	AX_INLINE void _advance(axInt n) {
		_remain = _remain.sliceFrom(n);
	}

	axByteSpan _buf;
	axByteSpan _remain;
};

inline
void axBinDeserializer::io(axIString8& value) {
	axInt len = 0;
	io(len);

	if (len > remain().size()) {
		// prevent insane len cause out of memory attack
		throw axError_Undefined(AX_LOC);
	}

	value.resize(len);
	io_raw(axByteMutSpan(reinterpret_cast<axByte*>(value.data()), len));
}

inline
void axBinDeserializer::io(axIByteArray& value) {
	axInt len = 0;
	io(len);

	if (len > remain().size()) {
		// prevent insane len cause out of memory attack
		throw axError_Undefined(AX_LOC);
	}

	value.resize(len);
	io_raw(value.mutSpan());
}

template<class T, axInt N>
void axBinDeserializer_handler(axBinDeserializer& se, axStringT<T, N>& value) {
	se.io(value.asIString());
}

template<class CLOCK>
void axBinDeserializer_handler(axBinDeserializer& se, axTimestampT<CLOCK>& value) {
	axInt64 t;
	se.io(t);
	value.setInt(t);
}

template<class T>
void axBinDeserializer_handler(axBinDeserializer& se, T& value) {
	value.onSerialize(se);
}
