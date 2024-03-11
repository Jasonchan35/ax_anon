#pragma once

namespace axTypeTraits {
	template<class A, class B> struct isSame			{ static const bool value = std::is_same<A,B>::value; };
	template<class B, class T> struct isBaseOf			{ static const bool value = std::is_base_of<B,T>::value; };

	template<class T> struct isPod						{ static const bool value = std::is_pod<T>::value;  };
	// safely copied with std::memcpy or serialized 
	template<class T> struct isTriviallyCopyable		{ static const bool value = std::is_trivially_copyable<T>::value; };
	// only works for data type without padding
	template<class T> struct isTriviallyComparable		{ static const bool value = false; };

	template<class T> struct isLiteralType				{ static const bool value = std::is_literal_type<T>::value;  };
	template<class T> struct isStandardLayout			{ static const bool value = std::is_standard_layout<T>::value; };

	template<class T> struct isPolymorphic				{ static const bool value = std::is_polymorphic<T>::value; };
	template<class T> struct hasVirtualDestructor		{ static const bool value = std::has_virtual_destructor<T>::value; };
	template<class T> struct isPointer					{ static const bool value = std::is_pointer<T>::value;  };

	template<class From, class To> struct isConvertible	{ static const bool value = std::is_convertible<From, To>::value; };

	template<class T> struct isConst					{ static const bool value = std::is_const<T>::value; };

	template<class T> struct isIntegral					{ static const bool value = std::is_integral<T>::value; };
	template<class T> struct isFloatingPoint			{ static const bool value = std::is_floating_point<T>::value; };
	template<class T> struct isSigned					{ static const bool value = std::is_signed<T>::value; };
	template<class T> struct isUnsigned					{ static const bool value = std::is_unsigned<T>::value; };

	template<class T> struct isUnsignedInt				{ static const bool value = isUnsigned<T>::value && isIntegral<T>::value; };

//	template<class Func> struct invokeResult			{ using Type = typename std::invoke_result<Func>::type; };	// doesn't support on Android NDK
	template<class Func> struct resultOf				{ using Type = typename std::result_of<Func>::type; };		// removed in C++20

	template<class T> struct removeRef					{ using Type = T; };
	template<class T> struct removeRef<T&>				{ using Type = T; };
	template<class T> struct removeRef<T&&>				{ using Type = T; };

	template<class T> struct removeConst				{ using Type = T; };
	template<class T> struct removeConst<const T>		{ using Type = T; };

	template<class T> struct removeConstRef				{ using Type = T; };
	template<class T> struct removeConstRef<T&>			{ using Type = T; };
	template<class T> struct removeConstRef<T&&>		{ using Type = T; };
	template<class T> struct removeConstRef<const T>	{ using Type = T; };
	template<class T> struct removeConstRef<const T&>	{ using Type = T; };

	template<class T> struct underlying					{ using Type = typename std::underlying_type<T>::type; };

	template<class T> struct spanType					{ using Type = T; };
	template<class T> struct mutSpanType				{ using Type = T; };
	template<class T> struct interfaceType				{ using Type = T; };

	template<class T>
	struct memberType {};

	template<class Obj, class Member>
	struct memberType<Member Obj::*> { using Type = Member; };

	template<class T> struct digits10					{ const static axInt value = std::numeric_limits<T>::digits10; };

	//--------
	template<axInt N> struct typeBySize;

	template<> struct typeBySize<1> {
		typedef axInt8		IntType;
		typedef axUInt8		UIntType;
		typedef axChar8		CharType;
	};

	template<> struct typeBySize<2> {
		typedef axInt16		IntType;
		typedef axUInt16	UIntType;
		typedef axChar16	CharType;
	};

	template<> struct typeBySize<4> {
		typedef axInt32		IntType;
		typedef axUInt32	UIntType;
		typedef axFloat32	FloatType;
		typedef axChar32	CharType;
	};

	template<> struct typeBySize<8> {
		typedef axInt64		IntType;
		typedef axUInt64	UIntType;
		typedef axFloat64	FloatType;
	};

	template<> struct typeBySize<16> {
		typedef axFloat128	FloatType;
	};

	//-----------------------
	template<class TYPE_TRUE, class TYPE_FALSE, bool COND> struct selectType;
	template<class TYPE_TRUE, class TYPE_FALSE> struct selectType<TYPE_TRUE, TYPE_FALSE, true > { using Type = TYPE_TRUE; };
	template<class TYPE_TRUE, class TYPE_FALSE> struct selectType<TYPE_TRUE, TYPE_FALSE, false> { using Type = TYPE_FALSE; };

	template<class T, bool COND> struct toConst     { using Type = typename selectType<const T, T, COND>::Type;  };
	template<class T, class B>   struct selectConst { using Type = typename toConst<T, isConst<B>::value>::Type; };

	//-----------------------
	template<class... Ts> struct selectLast {
		using Type = typename std::tuple_element< sizeof...(Ts)-1, std::tuple<Ts...> >::type;
	};

	//-----------------------
	template<class T> constexpr typename underlying<T>::Type  castUnderlying(const T& v) { return static_cast<     typename underlying<T>::Type >(v); }
	template<class T> constexpr typename underlying<T>::Type* castUnderlyingPtr(   T* v) { return reinterpret_cast<typename underlying<T>::Type*>(v); }

	template<class T> struct makeSigned;
	template<> struct makeSigned<axInt8  > { using Type = axInt8;  };
	template<> struct makeSigned<axInt16 > { using Type = axInt16; };
	template<> struct makeSigned<axInt32 > { using Type = axInt32; };
	template<> struct makeSigned<axInt64 > { using Type = axInt64; };
	template<> struct makeSigned<axUInt8 > { using Type = axInt8;  };
	template<> struct makeSigned<axUInt16> { using Type = axInt16; };
	template<> struct makeSigned<axUInt32> { using Type = axInt32; };
	template<> struct makeSigned<axUInt64> { using Type = axInt64; };

	template<class T> struct makeUnsigned;
	template<> struct makeUnsigned<axInt8  > { using Type = axUInt8;  };
	template<> struct makeUnsigned<axInt16 > { using Type = axUInt16; };
	template<> struct makeUnsigned<axInt32 > { using Type = axUInt32; };
	template<> struct makeUnsigned<axInt64 > { using Type = axUInt64; };
	template<> struct makeUnsigned<axUInt8 > { using Type = axUInt8;  };
	template<> struct makeUnsigned<axUInt16> { using Type = axUInt16; };
	template<> struct makeUnsigned<axUInt32> { using Type = axUInt32; };
	template<> struct makeUnsigned<axUInt64> { using Type = axUInt64; };

	template<class T> typename makeUnsigned<T>::Type castUnsigned(const T& v) { return static_cast<typename makeUnsigned<T>::Type>(v); }
	template<class T> typename   makeSigned<T>::Type   castSigned(const T& v) { return static_cast<typename   makeSigned<T>::Type>(v); }

	template<class T> struct makeFloat;
	template<> struct makeFloat<axInt8    > { using Type = axFloat32;  };
	template<> struct makeFloat<axInt16   > { using Type = axFloat32;  };
	template<> struct makeFloat<axInt32   > { using Type = axFloat64;  };
	template<> struct makeFloat<axInt64   > { using Type = axFloat64;  };
	template<> struct makeFloat<axUInt8   > { using Type = axFloat32;  };
	template<> struct makeFloat<axUInt16  > { using Type = axFloat32;  };
	template<> struct makeFloat<axUInt32  > { using Type = axFloat64;  };
	template<> struct makeFloat<axUInt64  > { using Type = axFloat64;  };
	template<> struct makeFloat<axFloat32 > { using Type = axFloat32;  };
	template<> struct makeFloat<axFloat64 > { using Type = axFloat64;  };
	template<> struct makeFloat<axFloat128> { using Type = axFloat128; };

	template<class T> struct defaultValue;
	template<class T> struct defaultValue { static void get(T& data) { data.setToDefaultValue(); } };

	template<> struct defaultValue<axInt8    >	{ static void get(axInt8     & data) { data = 0; } };
	template<> struct defaultValue<axInt16   >	{ static void get(axInt16    & data) { data = 0; } };
	template<> struct defaultValue<axInt32   >	{ static void get(axInt32    & data) { data = 0; } };
	template<> struct defaultValue<axInt64   >	{ static void get(axInt64    & data) { data = 0; } };

	template<> struct defaultValue<axUInt8   >	{ static void get(axUInt8    & data) { data = 0; } };
	template<> struct defaultValue<axUInt16  >	{ static void get(axUInt16   & data) { data = 0; } };
	template<> struct defaultValue<axUInt32  >	{ static void get(axUInt32   & data) { data = 0; } };
	template<> struct defaultValue<axUInt64  >	{ static void get(axUInt64   & data) { data = 0; } };

	template<> struct defaultValue<axFloat32 >	{ static void get(axFloat32  & data) { data = 0; } };
	template<> struct defaultValue<axFloat64 >	{ static void get(axFloat64  & data) { data = 0; } };
	template<> struct defaultValue<axFloat128>	{ static void get(axFloat128 & data) { data = 0; } };

} //axTypeTraits

template<class T>
struct axNumLimit {
	AX_INLINE static constexpr T defaultValue()	{ return 0; };
	AX_INLINE static constexpr T lowest()			{ return std::numeric_limits<T>::lowest(); };
	AX_INLINE static constexpr T min()				{ return std::numeric_limits<T>::min(); };
	AX_INLINE static constexpr T max()				{ return std::numeric_limits<T>::max(); };
	AX_INLINE static constexpr T inf()				{ return std::numeric_limits<T>::infinity(); };
	AX_INLINE static constexpr T nan()				{ return std::numeric_limits<T>::quiet_NaN(); }
};

const axInt     axInt_max     = axNumLimit<axInt>::max();

const axFloat32 axFloat32_max = axNumLimit<axFloat32>::max();
const axFloat32 axFloat32_inf = axNumLimit<axFloat32>::inf();
const axFloat32 axFloat32_nan = axNumLimit<axFloat32>::nan();

const axFloat64 axFloat64_max = axNumLimit<axFloat64>::max();
const axFloat64 axFloat64_inf = axNumLimit<axFloat64>::inf();
const axFloat64 axFloat64_nan = axNumLimit<axFloat64>::nan();

using axCharW_Native = typename axTypeTraits::typeBySize<sizeof(wchar_t)>::CharType;
AX_STATIC_ASSERT(sizeof(axCharW) == sizeof(axCharW_Native));

AX_INLINE	const	axCharW_Native&		axCharW_to_Native(const axCharW&  v) { return reinterpret_cast<const axCharW_Native& >(v); }
AX_INLINE	const	axCharW_Native*&	axCharW_to_Native(const axCharW*& v) { return reinterpret_cast<const axCharW_Native*&>(v); }
AX_INLINE			axCharW_Native		axCharW_to_Native(      axCharW   v) { return static_cast<           axCharW_Native  >(v); }
AX_INLINE			axCharW_Native&		axCharW_to_Native(      axCharW&  v) { return reinterpret_cast<      axCharW_Native& >(v); }
AX_INLINE			axCharW_Native*&	axCharW_to_Native(      axCharW*& v) { return reinterpret_cast<      axCharW_Native*&>(v); }

using axIntPtr  = typename axTypeTraits::typeBySize<sizeof(void*)>::IntType;
using axUIntPtr = typename axTypeTraits::typeBySize<sizeof(void*)>::UIntType;

template<class C, class I = typename axTypeTraits::typeBySize<sizeof(C)>::IntType > const I* axCharPtrToIntPtr(const C* p) { return reinterpret_cast<const I*>(p); }
template<class C, class I = typename axTypeTraits::typeBySize<sizeof(C)>::IntType >       I* axCharPtrToIntPtr(      C* p) { return reinterpret_cast<      I*>(p); }

template<class C, class I = typename axTypeTraits::typeBySize<sizeof(C)>::UIntType> const I* axCharPtrToUIntPtr(const C* p) { return reinterpret_cast<const I*>(p); }
template<class C, class I = typename axTypeTraits::typeBySize<sizeof(C)>::UIntType>       I* axCharPtrToUIntPtr(      C* p) { return reinterpret_cast<      I*>(p); }

//---- template specialization -------

// --- Unsigned int ---
#define	AX_MACRO_OP(T) \
	constexpr	bool	ax_less_than0(T)	{ return false; } \
	namespace axTypeTraits { \
		template<> struct isTriviallyCopyable<T>	{ static const bool value = true; }; \
	} \
//-------
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_EXTRA_NATIVE_UINT(AX_MACRO_OP);
#undef AX_MACRO_OP

// --- Signed int ---
#define	AX_MACRO_OP(T)	\
	constexpr	bool	ax_less_than0( T value )		{ return value < 0; } \
	namespace axTypeTraits { \
		template<> struct isTriviallyCopyable<T>	{ static const bool value = true; }; \
	} \
//-------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_EXTRA_NATIVE_SINT(AX_MACRO_OP);
#undef AX_MACRO_OP

// --- Floating point ---
#define	AX_MACRO_OP(T)	\
	constexpr	bool	ax_less_than0( T value )		{ return value < 0; } \
	namespace axTypeTraits { \
		template<> struct isTriviallyCopyable<T>	{ static const bool value = true; }; \
	} \
//-------
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP
	


