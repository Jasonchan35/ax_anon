//
//  basic_types.h
//  ax_core
//
//  Created by Jason on 2015-03-15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once
#include "ax_base.h"

using axNull = std::nullptr_t;
using ax_exception = std::exception;

typedef uint8_t				axByte;

typedef int8_t				axInt8;
typedef int16_t				axInt16;
typedef int32_t				axInt32;
typedef int64_t				axInt64;

typedef uint8_t				axUInt8;
typedef uint16_t			axUInt16;
typedef uint32_t			axUInt32;
typedef uint64_t			axUInt64;

typedef long				axIntL;
typedef long long			axIntLL;

typedef unsigned long		axUIntL;
typedef unsigned long long	axUIntLL;

//typedef half				axFloat16;
typedef float				axFloat32;
typedef double				axFloat64;
typedef long double			axFloat128;

#if AX_CPU_LP64
	typedef int64_t		axInt;
	typedef uint64_t	axUInt;
#elif AX_CPU_LP32
	typedef int32_t		axInt;
	typedef uint32_t	axUInt;
#else
	#error
#endif

constexpr size_t axInt_to_size_t(axInt  v) { return static_cast<size_t>(v < 0 ? 0 : v); }
constexpr axInt  axInt_from_size_t(size_t v) { auto t = static_cast<axInt>(v); return t < 0 ? 0 : t; }

template<class T> inline constexpr	typename std::underlying_type<T>::type	ax_enum_int(           T value)	{ return       static_cast<typename std::underlying_type<T>::type >( value); }
template<class T> inline constexpr	typename std::underlying_type<T>::type&	ax_enum_int_ref(      T& value)	{ return *reinterpret_cast<typename std::underlying_type<T>::type*>(&value); }

template<class T> inline constexpr	typename std::underlying_type<T>::type const & ax_enum_int_ref(const T& value) {
	return *reinterpret_cast<const typename std::underlying_type<T>::type*>(&value);
}

template<class T> inline bool ax_compare(const T& a, const T& b) { return a < b; }

#if 0 //using axCharT
	template<class T>
	class axCharT {
	public:
		using CharType = T;

		axCharT() = default;
		axCharT(T v) : _v(v) {}
		operator T() const { return _v; }
	private:
		T _v;
	};

	using axChar8   = axCharT<char>; // char8_t: require c++20
	using axChar16  = axCharT<char16_t>;
	using axChar32  = axCharT<char32_t>;
	using axCharW   = axCharT<wchar_t>;
	using axCharU   = axCharT<axChar32>;

#else
	using axChar8   = char; // char8_t: require c++20
	using axChar16  = char16_t;
	using axChar32  = char32_t;
	using axCharW   = wchar_t;
	using axCharU   = axChar32; // unicode code point
#endif

#if AX_CPU_ENDIAN_LITTLE
	constexpr char32_t axFourCC(char a, char b, char c, char d ) {
		return static_cast<char32_t>(a)
		     | static_cast<char32_t>(b) << 8
		     | static_cast<char32_t>(c) << 16
		     | static_cast<char32_t>(d) << 24;
	}
#elif AX_CPU_ENDIAN_BIG
	constexpr char32_t axFourCC(char a, char b, char c, char d ) {
		return static_cast<char32_t>(a) << 24
			 | static_cast<char32_t>(b) << 16
			 | static_cast<char32_t>(c) << 8
			 | static_cast<char32_t>(d);
	}
#else
	#error "Unknown Host Endian"
#endif

constexpr char32_t axFourCC(const char (&sz)[5]) { return axFourCC(sz[0], sz[1], sz[2], sz[3]); }

#if AX_COMPILER_VC
	#define AX_SZ_8_JOIN(A,B)	(""     A ## B)
	#define AX_SZ_16_JOIN(A,B)	(u"" u##A ## u##B)
	#define AX_SZ_32_JOIN(A,B)	(U"" U##A ## u##B)
	#define AX_SZ_W_JOIN(A,B)	(L"" L##A ## L##B)

	#define AX_SZ_8( sz)			(""   ##sz)
	#define AX_SZ_16(sz)			(u"" u##sz)
	#define AX_SZ_32(sz)			(U"" U##sz)
    #define AX_SZ_W( sz)			(L"" L##sz)

	#define AX_CH_8( c)			axChar8(    c)
    #define AX_CH_16(c)			axChar16(u##c)
    #define AX_CH_32(c)			axChar32(U##c)
    #define AX_CH_W( c)			axCharW( L##c)

#else
	#define AX_SZ_8_JOIN(A,B)	(""  A ## B)
	#define AX_SZ_16_JOIN(A,B)	(u"" A ## B)
	#define AX_SZ_32_JOIN(A,B)	(U"" A ## B)
	#define AX_SZ_W_JOIN(A,B)	(L"" A ## B)

	#define AX_SZ_8( sz)			(""  sz)
	#define AX_SZ_16(sz)			(u"" sz)
    #define AX_SZ_32(sz)			(U"" sz)
    #define AX_SZ_W( sz)			(L"" sz)

    #define AX_CH_8( c)			axChar8(    c)
    #define AX_CH_16(c)			axChar16(u##c)
    #define AX_CH_32(c)			axChar32(U##c)
    #define AX_CH_W( c)			axCharW (L##c)

#endif

#define AX_SZ_U  AX_SZ_32
#define AX_CH_U  AX_CH_32

#ifdef AX_GEN_CONFIG_Debug
	#define AX_DEBUG_SZ_8( sz)	AX_SZ_8( sz)
	#define AX_DEBUG_SZ_16(sz)	AX_SZ_16(sz)
	#define AX_DEBUG_SZ_32(sz)	AX_SZ_32(sz)
	#define AX_DEBUG_SZ_W( sz)	AX_SZ_W( sz)
	#define AX_DEBUG_SZ_U( sz)	AX_SZ_U( sz)
#else
	#define AX_DEBUG_SZ_8( sz)	AX_SZ_8( "")
	#define AX_DEBUG_SZ_16(sz)	AX_SZ_16("")
	#define AX_DEBUG_SZ_32(sz)	AX_SZ_32("")
	#define AX_DEBUG_SZ_W( sz)	AX_SZ_W( "")
	#define AX_DEBUG_SZ_U( sz)	AX_SZ_U( "")
#endif

#define AX_STR_8(  sz )	(axStrView8 (AX_SZ_8 (sz)))
#define AX_STR_16( sz )	(axStrView16(AX_SZ_16(sz)))
#define AX_STR_32( sz )	(axStrView32(AX_SZ_32(sz)))
#define AX_STR_W(  sz )	(axStrViewW (AX_SZ_W (sz)))
#define AX_STR_U(  sz )	(axStrViewU (AX_SZ_U (sz)))

//#define ax_tmp_utf_str8( name, src ) axTempString8 name; name.setUtf( src );
//#define ax_tmp_utf_strW( name, src ) axTempStringW name; name.setUtf( src );
//#define ax_tmp_utf_strU( name, src ) axTempStringU name; name.setUtf( src );

#define AX_TYPE_CHAR_DEFINE_8   10001
#define AX_TYPE_CHAR_DEFINE_16  10002
#define AX_TYPE_CHAR_DEFINE_32  10003
#define AX_TYPE_CHAR_DEFINE_W   10004

//define axChar type
#define AX_TYPE_CHAR_DEFINE AX_TYPE_CHAR_DEFINE_8

#if AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_8
	typedef axChar8			axChar;
	#define AX_SZ_join(sz)	AX_SZ_8_JOIN(sz)
	#define AX_SZ(sz)		AX_SZ_8(sz)
	#define AX_CH(sz)		AX_CH_8(sz)
	#define AX_STR(sz)		AX_STR_8(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_16
	typedef axChar16		axChar;
	#define AX_SZ_join(sz)	AX_SZ_16_JOIN(sz)
	#define AX_SZ(sz)		AX_SZ_16(sz)
	#define AX_CH(sz)		AX_CH_16(sz)
	#define AX_STR(sz)		AX_STR_16(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_32
    typedef axChar32		axChar;
	#define AX_SZ_join(sz)	AX_SZ_32_JOIN(sz)
    #define AX_SZ(sz)		AX_SZ_32(sz)
    #define AX_CH(sz)		AX_CH_32(sz)
    #define AX_STR(sz)		AX_STR_32(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_W
	typedef axCharW			axChar;
	#define AX_SZ_join(sz)	AX_SZ_W_JOIN(sz)
	#define AX_SZ(sz)		AX_SZ_W(sz)
	#define AX_CH(sz)		AX_CH_W(sz)
	#define AX_STR(sz)		AX_STR_W(sz)

#else
	#error
#endif

class axType;
class axFormat;
class axJsonWriter;
class axJsonReader;

class axBinSerializer;
class axBinDeserializer;

class axScopedPerfTiming;
template<class T> class axIArray;

class axSourceLoc {
public:

	AX_INLINE
	axSourceLoc(const char* func_ = "", const char* file_ = "", int line_ = 0) {
		func = func_;
		file = file_;
		line = line_;
	}

	explicit operator bool() const { return file[0] != 0; }
	void onFormat(axFormat& f) const;
	void onJsonWriter(axJsonWriter& wr) const;

	void onSerialize(axBinSerializer& se);
	void onSerialize(axBinDeserializer& se) {};

	const char*	func;
	const char*	file;
	int			line;
};

AX_INLINE
std::ostream& operator<< (std::ostream& s, const axSourceLoc& loc) {
	s << "func: " << loc.func << "file: " << loc.file << " line:" << loc.line;
	return s;
}

#include "axError.h"

#define	AX_TYPE_LIST_BOOL(op) \
	op( bool )		\
//------

#if AX_TYPE_HAS_SIZE_T
	#define	AX_TYPE_LIST_SIZE_T(op)		op( size_t )
	#define	AX_TYPE_LIST_SSIZE_T(op)	op( ssize_t )
#else
	#define	AX_TYPE_LIST_SIZE_T(...)
	#define	AX_TYPE_LIST_SSIZE_T(...)
#endif

//'long' is native type
#if AX_TYPE_HAS_LONG
	#define	AX_TYPE_LIST_LONG(op)		op( axIntL  )
	#define	AX_TYPE_LIST_ULONG(op)		op( axUIntL )
#else
	#define	AX_TYPE_LIST_LONG(...)  	//not use
	#define	AX_TYPE_LIST_ULONG(...)  	//not use
#endif

//'long long' is native type
#if AX_TYPE_HAS_LONGLONG
	#define	AX_TYPE_LIST_LONGLONG(op)	op( axIntLL )
	#define	AX_TYPE_LIST_ULONGLONG(op)	op( axUIntLL )
#else
	#define	AX_TYPE_LIST_LONGLONG(...)  //not use
	#define	AX_TYPE_LIST_ULONGLONG(...) //not use
#endif

#define	AX_TYPE_LIST_BOOL(op) \
	op( bool ) \
//-----

#define	AX_TYPE_LIST_CHAR_OTHER_THAN_CHAR8(op) \
	op( axCharW  ) \
	op( axChar16 ) \
	op( axChar32 ) \
//----

#define	AX_TYPE_LIST_CHAR(op) \
	op( axChar8 ) \
	AX_TYPE_LIST_CHAR_OTHER_THAN_CHAR8(op) \
//----

#define	AX_TYPE_LIST_SINT(op) \
	op( axInt8  ) \
	op( axInt16 ) \
	op( axInt32 ) \
	op( axInt64 ) \
//----

#define	AX_TYPE_LIST_UINT(op) \
	op( axUInt8  ) \
	op( axUInt16 ) \
	op( axUInt32 ) \
	op( axUInt64 ) \
//----

#define	AX_TYPE_LIST_FLOAT(op) \
	op( axFloat32  ) \
	op( axFloat64  ) \
	op( axFloat128 ) \
//----

#define AX_TYPE_LIST_ALL_PRIMITIVES(op) \
	AX_TYPE_LIST_BOOL(op) \
	AX_TYPE_LIST_SINT(op) \
	AX_TYPE_LIST_UINT(op) \
	AX_TYPE_LIST_FLOAT(op) \
	AX_TYPE_LIST_CHAR(op) \
//----

#define AX_TYPE_LIST_EXTRA_NATIVE_SINT(op) \
	AX_TYPE_LIST_LONG(op) \
	AX_TYPE_LIST_LONGLONG(op) \
	AX_TYPE_LIST_SSIZE_T(op) \
//---------

#define AX_TYPE_LIST_EXTRA_NATIVE_UINT(op) \
	AX_TYPE_LIST_ULONG(op) \
	AX_TYPE_LIST_ULONGLONG(op) \
	AX_TYPE_LIST_SIZE_T(op) \
//---------

//----- Forward declaration for String
const bool  axString_kEnableSmallStringOptimization = true;
const axInt axString_kDefaultSize = 64;
const axInt axTempString_kSize = 512 - 48; // 40 from sizeof(axIString<>)  + vtable
const axInt axSmallString_kSize = axString_kEnableSmallStringOptimization ? 0 : 15;

template<class T> class axStrMutViewT;
template<class T> class axStrLiteralT;
template<class T> using axStrViewT = axStrMutViewT<const T>;

template<class T> class axIStringT;
template<class T, axInt N> class axStringT;
template<class T, axInt N> class axFormatT;

template<class T> class axPersistStringT;

using axStrMutView		= axStrMutViewT<  axChar   >;
using axStrMutViewW		= axStrMutViewT<  axCharW  >;
using axStrMutViewU		= axStrMutViewT<  axCharU  >;
using axStrMutView8		= axStrMutViewT<  axChar8  >;
using axStrMutView16	= axStrMutViewT<  axChar16 >;
using axStrMutView32	= axStrMutViewT<  axChar32 >;

using axStrView			= axStrViewT< axChar    >;
using axStrViewW		= axStrViewT< axCharW   >;
using axStrViewU		= axStrViewT< axCharU   >;
using axStrView8		= axStrViewT< axChar8   >;
using axStrView16		= axStrViewT< axChar16  >;
using axStrView32		= axStrViewT< axChar32  >;

using axStrLiteral		= axStrLiteralT< axChar   >;
using axStrLiteralW		= axStrLiteralT< axCharW  >;
using axStrLiteralU		= axStrLiteralT< axCharU  >;
using axStrLiteral32	= axStrLiteralT< axChar32 >;
using axStrLiteral8		= axStrLiteralT< axChar8  >;
using axStrLiteral16	= axStrLiteralT< axChar16 >;

using axIString			= axIStringT< axChar    >;
using axIStringW		= axIStringT< axCharW   >;
using axIStringU		= axIStringT< axCharU   >;
using axIString8		= axIStringT< axChar8   >;
using axIString16		= axIStringT< axChar16  >;
using axIString32		= axIStringT< axChar32  >;

using axString			= axStringT< axChar,    axString_kDefaultSize >;
using axStringW			= axStringT< axCharW,   axString_kDefaultSize >;
using axStringU			= axStringT< axCharU,   axString_kDefaultSize >;
using axString8			= axStringT< axChar8,   axString_kDefaultSize >;
using axString16		= axStringT< axChar16,  axString_kDefaultSize >;
using axString32		= axStringT< axChar32,  axString_kDefaultSize >;

template<class T> using axTempStringT = axStringT<T, axTempString_kSize>;
using axTempString		= axTempStringT< axChar    >;
using axTempStringW		= axTempStringT< axCharW   >;
using axTempStringU		= axTempStringT< axCharU   >;
using axTempString8		= axTempStringT< axChar8   >;
using axTempString16	= axTempStringT< axChar16  >;
using axTempString32	= axTempStringT< axChar32  >;

template<class T> using axSmallStringT = axStringT<T, axSmallString_kSize>;
using axSmallString		= axSmallStringT< axChar    >;
using axSmallStringW	= axSmallStringT< axCharW   >;
using axSmallStringU	= axSmallStringT< axCharU   >;
using axSmallString8	= axSmallStringT< axChar8   >;
using axSmallString16	= axSmallStringT< axChar16  >;
using axSmallString32	= axSmallStringT< axChar32  >;

template< axInt N > using axString_		= axStringT<axChar,    N>;
template< axInt N > using axStringW_	= axStringT<axCharW,   N>;
template< axInt N > using axStringU_	= axStringT<axCharU,   N>;
template< axInt N > using axString8_	= axStringT<axChar8,   N>;
template< axInt N > using axString16_	= axStringT<axChar16,  N>;
template< axInt N > using axString32_	= axStringT<axChar32,  N>;

class axFormat;
class axFormatParam;

#include "axTypeTraits.h"

class axJsonReader;
template<class T> void axJsonReader_VoidPtrWrapper(axJsonReader& rd, void* data);

class axJsonWriter;
template<class T> void axJsonWriter_VoidPtrWrapper(axJsonWriter& wr, const void* data);

template<class TAG, class BASE>
class axDistinct : public BASE {
	using This = axDistinct;
	using Base = BASE;
public:
	axDistinct() = default;
	axDistinct(const axDistinct& r) { operator=(r); }

	static This s_from(const Base& v) { return This(v); }

	      Base& asBase()       { return *this; }
	const Base& asBase() const { return *this; }

	template<class SE>
	void onSerialize(SE& se) {
		se.io(*static_cast<Base*>(this));
	}

private:
	axDistinct(const Base& v) : Base(v) {}
};
