//
//  axUtfUtil.h
//  ax_core
//
//  Created by Jason Chan on 3/16/15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

#include "../container/axArrayUtil.h"

struct axUtfUtil : public axStaticClass {
#define AX_MACRO_OP(T) \
	AX_INLINE static void	convert(axIStringT<T>& dst, axStrView8  src) { _convert(dst, src); } \
	AX_INLINE static void	convert(axIStringT<T>& dst, axStrView16 src) { _convert(dst, src); } \
	AX_INLINE static void	convert(axIStringT<T>& dst, axStrView32 src) { _convert(dst, src); } \
	AX_INLINE static void	convert(axIStringT<T>& dst, axStrViewW  src) { _convert(dst, src); } \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP

	template<class SRC, class DST> AX_INLINE
	static axInt	getConvertedCount ( const SRC* src, axInt src_len ) {
		if (sizeof(DST) == sizeof(SRC)) {
			return src_len;
		}

		axInt out_len = 0;
		auto* srcEnd = src + src_len;
		for (axInt i = 0; i < src_len; i++) {
			if (src >= srcEnd) break;
			out_len += _utfCount<DST>(_decodeUtf(src, srcEnd));
		}
		return out_len;
	}

private:

	template< class SRC, class DST > inline
	static void _convert( axIStringT<DST>& dst, axStrViewT<SRC> src) {
		if (axTypeTraits::isSame<DST, SRC>::value) {
			auto  old_size = dst.size();
			dst.resize(old_size + src.size());
			auto* d = dst.data() + old_size;
			ax_memcpy(d, src.data(), src.sizeInBytes());
		} else {
			auto  src_len = src.size();
			const auto* s = src.begin();
			const auto* e = src.end();
			for (axInt i = 0; i < src_len; i++) {
				if (s >= e) break;
				_encodeUtf(dst, _decodeUtf(s, e));
			}
		}
	}

	template< class SRC > static axInt		_utfCount 	( axCharU v );
	template< class SRC > static axCharU 	_decodeUtf	( const SRC* & src, const SRC* e );
	template< class DST > static void 		_encodeUtf	( axIStringT<DST>& dst, axCharU v );
};

template<> AX_INLINE
axCharU axUtfUtil::_decodeUtf<axChar32>( const axChar32* & src, const axChar32* /* end */ ) {
	AX_STATIC_ASSERT(sizeof(axChar32) == sizeof(axCharU));
	axCharU a = *src; src++;
	return a;
}

template<> AX_INLINE
axCharU axUtfUtil::_decodeUtf<axChar16>( const axChar16* & src, const axChar16* end ) {
	auto v = static_cast<uint16_t>(*src);

	if (v >= 0xD800 && v < 0xDBFF) {
		if( src+2 > end ) throw axStringError_Utf();
		axCharU a = static_cast<uint16_t>(*src); src++;
		axCharU b = static_cast<uint16_t>(*src); src++;
		return ((a - 0xD800) << 10) | (b - 0xDC00);
	}

	axCharU a = static_cast<uint16_t>(*src); src++;
	return a;
}

template<> AX_INLINE
axCharU axUtfUtil::_decodeUtf<axChar8>( const axChar8* & src, const axChar8* end ) {
	auto v = static_cast<uint8_t>(*src);
	axCharU o = 0;
	if (v < 0x80) {
		o = static_cast<uint8_t>(*src); src++;
		return o;
	}

	if ((v & 0xE0) == 0xC0) {
		if( src+2 > end ) throw axStringError_Utf();
		o += (static_cast<uint8_t>(*src) & 0x1FU) << 6; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);      src++;
		return o;
	}

	if ((v & 0xF0) == 0xE0) {
		if( src+3 > end ) throw axStringError_Utf();
		o += (static_cast<uint8_t>(*src) & 0x0FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	if ((v & 0xF8) == 0xF0) {
		if( src+4 > end ) throw axStringError_Utf();
		o += (static_cast<uint8_t>(*src) & 0x07U) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	if ((v & 0xFC) == 0xF8) {
		if( src+5 > end ) throw axStringError_Utf();
		o += (static_cast<uint8_t>(*src) & 0x03U) << 24; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
	}

	if ((v & 0xFE) == 0xFC) {
		if( src+6 > end ) throw axStringError_Utf();
		o += (static_cast<uint8_t>(*src) & 0x01U) << 30; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 24; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	{
		AX_ASSERT(false);
		o = static_cast<uint8_t>(*src); src++;
		return o;
	}
}

template<> AX_INLINE
axCharU axUtfUtil::_decodeUtf<axCharW>( const axCharW* & src, const axCharW* e ) {
	return _decodeUtf<axCharW_Native>(axCharW_to_Native(src), axCharW_to_Native(e) );
}

//----------------------------------------------------

template<> AX_INLINE
axInt axUtfUtil::_utfCount< axChar32 >(axCharU v) {
	return 1;
}

template<> AX_INLINE
axInt axUtfUtil::_utfCount< axChar16 >(axCharU ch) {
	auto v = static_cast<uint32_t>(ch);
	if( v <  0x10000 ) return 1;
	if( v < 0x110000 ) return 2;

	AX_ASSERT(false);
	return 1;
}

template<> AX_INLINE
axInt axUtfUtil::_utfCount< axChar8 >(axCharU ch) {
	auto v = static_cast<uint32_t>(ch);
	if( v <       0x80 ) return 1;
	if( v <    0x00800 ) return 2;
	if( v <    0x10000 ) return 3;
	if( v <   0x200000 ) return 4;
// The patterns below are not part of UTF-8, but were part of the first specification.
	if( v <  0x4000000 ) return 5;
	if( v < 0x80000000 ) return 6;

	AX_ASSERT(false);
	return 1;
}

template<> AX_INLINE
axInt axUtfUtil::_utfCount< axCharW >( axCharU v ) {
	return _utfCount<axCharW_Native>(v);
}

//----------------------------------------------------

template<> AX_INLINE
void axUtfUtil::_encodeUtf( axIStringT<axCharU>& dst, axCharU v ) {
	dst.append(v);
}

template<> AX_INLINE
void axUtfUtil::_encodeUtf( axIStringT<axChar16>& dst, axCharU ch) {
	auto v = static_cast<uint32_t>(ch);
	if( v <  0x10000 ) {
		dst.append(static_cast<axChar16>(v));
		return;
	}

	if( v < 0x110000 ) {
		axChar16 t[] = {
			static_cast<axChar16>(( v >> 10   ) + 0xD800),
			static_cast<axChar16>(( v & 0x3FF ) + 0xDC00),
			0
		};
		dst.append(t);
		return;
	}

	{
		AX_ASSERT(false);
		dst.append(static_cast<axChar16>(v));
		return;
	}
}

template<> AX_INLINE
void axUtfUtil::_encodeUtf< axChar8 >( axIStringT<axChar8>& dst, axCharU ch) {
	auto v = static_cast<uint32_t>(ch);
	if( v <       0x80 ) {
		dst.append(static_cast<axChar8>(v));
		return;
	}

	if( v <    0x00800 ) {
		axChar8 t[] = {
			static_cast<axChar8>(( v >> 6   ) | 0xC0),
			static_cast<axChar8>(( v & 0x3F ) | 0x80),
			0
		};
		dst.append(t);
		return;
	}

	if( v <    0x10000 ) {
		axChar8 t[] = {
			static_cast<axChar8>(( (v >> 12)        ) | 0xE0),
			static_cast<axChar8>(( (v >> 6 ) & 0x3F ) | 0x80),
			static_cast<axChar8>((  v        & 0x3F ) | 0x80),
			0
		};
		dst.append(t);
		return;
	}

	if( v <   0x200000 ) {
		axChar8 t[] = {
			static_cast<axChar8>(( (v >> 18)        ) | 0xF0),
			static_cast<axChar8>(( (v >> 12) & 0x3F ) | 0x80),
			static_cast<axChar8>(( (v >> 6 ) & 0x3F ) | 0x80),
			static_cast<axChar8>((  v        & 0x3F ) | 0x80),
			0
		};
		dst.append(t);
		return;
	}

// The patterns below are not part of UTF-8, but were part of the first specification.
	if( v <  0x4000000 ) {
		axChar8 t[] = {
			static_cast<axChar8>(((v >> 24)        ) | 0xF8),
			static_cast<axChar8>(((v >> 18) & 0x3F ) | 0x80),
			static_cast<axChar8>(((v >> 12) & 0x3F ) | 0x80),
			static_cast<axChar8>(((v >> 6 ) & 0x3F ) | 0x80),
			static_cast<axChar8>(( v        & 0x3F ) | 0x80),
			0
		};
		dst.append(t);
		return;
	}

	if( v < 0x80000000 ) {
		axChar8 t[] = {
			static_cast<axChar8>(((v >> 30)        ) | 0xFC),
			static_cast<axChar8>(((v >> 24) & 0x3F ) | 0x80),
			static_cast<axChar8>(((v >> 18) & 0x3F ) | 0x80),
			static_cast<axChar8>(((v >> 12) & 0x3F ) | 0x80),
			static_cast<axChar8>(((v >> 6 ) & 0x3F ) | 0x80),
			static_cast<axChar8>(( v        & 0x3F ) | 0x80),
			0
		};
		dst.append(t);
		return;
	}

	{
		AX_ASSERT(false);
		dst.append(static_cast<axChar8>(v));
	}
}

template<> AX_INLINE
void axUtfUtil::_encodeUtf< axCharW >( axIStringT<axCharW>& dst, axCharU v ) {
	_encodeUtf(reinterpret_cast< axIStringT<axCharW_Native>& >(dst), v );
}
