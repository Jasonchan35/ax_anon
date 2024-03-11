#pragma once

class axByteOrder {
public:
	class StraightUInt {
	public:
		constexpr static axUInt8  convert(axUInt8  v) { return v; }
		constexpr static axUInt16 convert(axUInt16 v) { return v; }
		constexpr static axUInt32 convert(axUInt32 v) { return v; }
		constexpr static axUInt64 convert(axUInt64 v) { return v; }
	};

	class ByteSwapUInt {
	public:
		constexpr static axUInt8  convert(axUInt8  v) {
			return v; 
		}
		constexpr static axUInt16 convert(axUInt16 v) {
			return (axUInt16)((v>>8) | (v<<8));
		}
		constexpr static axUInt32 convert(axUInt32 v) {
			v = ((v<<8) & 0xFF00FF00UL) | ((v>>8) & 0x00FF00FFUL); 
			return (v>>16) | (v<<16);
		}
		constexpr static axUInt64 convert(axUInt64 v) { 
			v = ((v<< 8) & 0xFF00FF00FF00FF00ULL) | ((v>> 8) & 0x00FF00FF00FF00FFULL);
			v = ((v<<16) & 0xFFFF0000FFFF0000ULL) | ((v>>16) & 0x0000FFFF0000FFFFULL);
			return (v>>32) | (v<<32);
		}
	};

	template<class ToHost, class FromHost>
	class Convertor {
	public:
	//to host
		constexpr static axUInt8    toHost(axUInt8    v) { return                         ToHost::convert(v); }
		constexpr static axUInt16   toHost(axUInt16   v) { return                         ToHost::convert(v); }
		constexpr static axUInt32   toHost(axUInt32   v) { return                         ToHost::convert(v); }
		constexpr static axUInt64   toHost(axUInt64   v) { return                         ToHost::convert(v); }
		constexpr static axInt8     toHost(axInt8     v) { return static_cast<axInt8    >(ToHost::convert(static_cast<axUInt8  >(v))); }
		constexpr static axInt16    toHost(axInt16    v) { return static_cast<axInt16   >(ToHost::convert(static_cast<axUInt16 >(v))); }
		constexpr static axInt32    toHost(axInt32    v) { return static_cast<axInt32   >(ToHost::convert(static_cast<axUInt32 >(v))); }
		constexpr static axInt64    toHost(axInt64    v) { return static_cast<axInt64   >(ToHost::convert(static_cast<axUInt64 >(v))); }

		AX_INLINE static axFloat32  toHost(axFloat32 v) { 
			axUInt32 i = *reinterpret_cast<axUInt32*>(&v);
			i = ToHost::convert(i);
			return *reinterpret_cast<axFloat32*>(&i);
		}
		
		AX_INLINE static axFloat64  toHost(axFloat64 v) {
			axUInt64 i = *reinterpret_cast<axUInt64*>(&v);
			i = ToHost::convert(i);
			return *reinterpret_cast<axFloat64*>(&i);
		}

	// from host
		constexpr static axUInt8    fromHost(axUInt8    v) { return                         FromHost::convert(v); }
		constexpr static axUInt16   fromHost(axUInt16   v) { return                         FromHost::convert(v); }
		constexpr static axUInt32   fromHost(axUInt32   v) { return                         FromHost::convert(v); }
		constexpr static axUInt64   fromHost(axUInt64   v) { return                         FromHost::convert(v); }
		constexpr static axInt8     fromHost(axInt8     v) { return static_cast<axInt8    >(FromHost::convert(static_cast<axUInt8  >(v))); }
		constexpr static axInt16    fromHost(axInt16    v) { return static_cast<axInt16   >(FromHost::convert(static_cast<axUInt16 >(v))); }
		constexpr static axInt32    fromHost(axInt32    v) { return static_cast<axInt32   >(FromHost::convert(static_cast<axUInt32 >(v))); }
		constexpr static axInt64    fromHost(axInt64    v) { return static_cast<axInt64   >(FromHost::convert(static_cast<axUInt64 >(v))); }

		AX_INLINE static axFloat32  fromHost(axFloat32 v) {
			axUInt32 i = *reinterpret_cast<axUInt32*>(&v);
			i = FromHost::convert(i);
			return *reinterpret_cast<axFloat32*>(&i);
		}
		AX_INLINE static axFloat64  fromHost(axFloat64 v) {
			axUInt64 i = *reinterpret_cast<axUInt64*>(&v);
			i = FromHost::convert(i);
			return *reinterpret_cast<axFloat64*>(&i);
		}
	};

	using StraightConvertor = Convertor<StraightUInt, StraightUInt>;
	using ByteSwapConvertor = Convertor<ByteSwapUInt, ByteSwapUInt>;

#if AX_CPU_ENDIAN_LITTLE
	using LittleEndian = StraightConvertor;
	using BigEndian    = ByteSwapConvertor;
	using HostEndian   = LittleEndian;

#elif AX_CPU_ENDIAN_BIG
	using LittleEndian = ByteSwapConvertor;
	using BigEndian    = StraightConvertor;
	using HostEndian   = BigEndian;

#else
	#error "Unknown Host Endian"
#endif


};
