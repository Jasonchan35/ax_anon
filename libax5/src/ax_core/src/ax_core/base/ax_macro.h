//
//  macro.h
//  ax_core
//
//  Created by Jason on 2014-11-21.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#define AX_COMMA ,
#define AX_EMPTY
#define AX_STRINGIFY(...)	#__VA_ARGS__

#define AX_IDENTITY(x) x
#define AX_CALL(M, ARGS) AX_IDENTITY( M(ARGS) )
// work around for MSVC bug - MSVC expands all __VA_ARGS__ as one argument
//#define ax_CALL_MACRO(MACRO_NAME, ARGS) MACRO_NAME ARGS

#define AX_FILE				axStrView8(__FILE__)
#define AX_FILE_BASENAME	axStrView_c_str(axCStr::fileBasename(AX_SZ(__FILE__)))
#define AX_LINE				static_cast<axInt>(__LINE__)
#define AX_LOC	axSourceLoc(AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__)

#define AX_JOIN_WORD_IMPL( w0,w1)		w0##w1
#define AX_JOIN_WORD3_IMPL(w0,w1,w2)	w0##w1##w2

#define AX_JOIN_WORD( w0,w1)			AX_JOIN_WORD_IMPL( w0, w1)
#define AX_JOIN_WORD3(w0,w1,w2)			AX_JOIN_WORD3_IMPL(w0, w1, w2)

#define AX_ENUM_OPERATORS(T) \
	constexpr T operator+(T a, std::underlying_type<T>::type b) { return static_cast<T>(ax_enum_int(a) + b); } \
	constexpr T operator-(T a, std::underlying_type<T>::type b) { return static_cast<T>(ax_enum_int(a) - b); } \
	constexpr T operator|(T a, std::underlying_type<T>::type b) { return static_cast<T>(ax_enum_int(a) | b); } \
	constexpr T operator&(T a, std::underlying_type<T>::type b) { return static_cast<T>(ax_enum_int(a) & b); } \
	constexpr T operator^(T a, std::underlying_type<T>::type b) { return static_cast<T>(ax_enum_int(a) ^ b); } \
	\
	constexpr T operator+(T a, T b) { return static_cast<T>(ax_enum_int(a) + ax_enum_int(b)); } \
	constexpr T operator-(T a, T b) { return static_cast<T>(ax_enum_int(a) - ax_enum_int(b)); } \
	constexpr T operator|(T a, T b) { return static_cast<T>(ax_enum_int(a) | ax_enum_int(b)); } \
	constexpr T operator&(T a, T b) { return static_cast<T>(ax_enum_int(a) & ax_enum_int(b)); } \
	constexpr T operator^(T a, T b) { return static_cast<T>(ax_enum_int(a) ^ ax_enum_int(b)); } \
	\
	constexpr T operator<<(T a, axInt b) { return static_cast<T>(ax_enum_int(a) << b); } \
	constexpr T operator>>(T a, axInt b) { return static_cast<T>(ax_enum_int(a) >> b); } \
	\
	constexpr T operator~(T a) { return static_cast<T>(~ax_enum_int(a)); } \
	\
	constexpr void operator+=(T& a, std::underlying_type<T>::type b) { a = a + b; } \
	constexpr void operator-=(T& a, std::underlying_type<T>::type b) { a = a - b; } \
	constexpr void operator|=(T& a, std::underlying_type<T>::type b) { a = a | b; } \
	constexpr void operator&=(T& a, std::underlying_type<T>::type b) { a = a & b; } \
	constexpr void operator^=(T& a, std::underlying_type<T>::type b) { a = a ^ b; } \
	\
	constexpr void operator<<=(T& a, axInt b) { a = a << b; } \
	constexpr void operator>>=(T& a, axInt b) { a = a >> b; } \
	\
	constexpr void operator+=(T& a, T b) { a = a + ax_enum_int(b); } \
	constexpr void operator-=(T& a, T b) { a = a - ax_enum_int(b); } \
	constexpr void operator|=(T& a, T b) { a = a | ax_enum_int(b); } \
	constexpr void operator&=(T& a, T b) { a = a & ax_enum_int(b); } \
	constexpr void operator^=(T& a, T b) { a = a ^ ax_enum_int(b); } \
	\
	constexpr T operator++(T& a)		{ a = static_cast<T>(ax_enum_int(a) + 1); return a; } \
	constexpr T operator--(T& a)		{ a = static_cast<T>(ax_enum_int(a) - 1); return a; } \
	\
	constexpr T operator++(T& a, int)	{ auto ret = a; a = static_cast<T>(ax_enum_int(a) + 1); return ret; } \
	constexpr T operator--(T& a, int)	{ auto ret = a; a = static_cast<T>(ax_enum_int(a) - 1); return ret; } \
//------------

#define AX_EVERY(value, every) (value % every == every - 1)

//====== AX_VA_ARGS_macroGen() from test_ax_macro.cpp ========

//! will always return >= 1, even AX_VA_ARGS_COUNT(), because it means one single empty arguemnt
#define AX_VA_ARGS_COUNT(...) AX_IDENTITY( AX_VA_ARGS_COUNT_INTERNAL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1) )
#define AX_VA_ARGS_COUNT_INTERNAL(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, COUNT, ...) COUNT

#define AX_VA_ARGS_FOR_EACH_1(MACRO, X) MACRO(X)
#define AX_VA_ARGS_FOR_EACH_2(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_1(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_3(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_2(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_4(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_3(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_5(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_4(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_6(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_5(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_7(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_6(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_8(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_7(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_9(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_8(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_10(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_9(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_11(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_10(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_12(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_11(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_13(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_12(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_14(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_13(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_15(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_14(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_16(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_15(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_17(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_16(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_18(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_17(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_19(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_18(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_20(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_19(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_21(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_20(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_22(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_21(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_23(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_22(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_24(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_23(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_25(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_24(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_26(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_25(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_27(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_26(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_28(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_27(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_29(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_28(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_30(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_29(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_31(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_30(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_32(MACRO, X, ...) MACRO(X) AX_IDENTITY(AX_VA_ARGS_FOR_EACH_31(MACRO, __VA_ARGS__))
//---------------------

#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_1(MACRO, X) MACRO(X)
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_2(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_1(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_3(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_2(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_4(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_3(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_5(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_4(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_6(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_5(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_7(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_6(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_8(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_7(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_9(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_8(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_10(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_9(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_11(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_10(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_12(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_11(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_13(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_12(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_14(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_13(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_15(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_14(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_16(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_15(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_17(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_16(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_18(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_17(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_19(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_18(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_20(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_19(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_21(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_20(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_22(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_21(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_23(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_22(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_24(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_23(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_25(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_24(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_26(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_25(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_27(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_26(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_28(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_27(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_29(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_28(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_30(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_29(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_31(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_30(MACRO, __VA_ARGS__))
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_32(MACRO, X, ...) MACRO(X), AX_IDENTITY(AX_VA_ARGS_FOR_EACH_WITH_COMMA_31(MACRO, __VA_ARGS__))

//====== End AX_VA_ARGS_macroGen() ========

#define AX_VA_ARGS_IS_EMPTY(...)  bool( sizeof("" #__VA_ARGS__) == 1 )

#define AX_VA_ARGS_FOR_EACH_SELECT(COUNT) AX_VA_ARGS_FOR_EACH_ ## COUNT
#define AX_VA_ARGS_FOR_EACH(MACRO, ...) AX_IDENTITY(AX_CALL(AX_VA_ARGS_FOR_EACH_SELECT, AX_VA_ARGS_COUNT(__VA_ARGS__)) (MACRO, __VA_ARGS__))
//----

#define AX_VA_ARGS_FOR_EACH_WITH_COMMA_SELECT(COUNT) AX_VA_ARGS_FOR_EACH_WITH_COMMA_ ## COUNT
#define AX_VA_ARGS_FOR_EACH_WITH_COMMA(MACRO, ...) AX_IDENTITY(AX_CALL(AX_VA_ARGS_FOR_EACH_WITH_COMMA_SELECT, AX_VA_ARGS_COUNT(__VA_ARGS__)) (MACRO, __VA_ARGS__))



#define	AX_DEBUG_SZ( a )	a
#define AX_UNUSED(v)	((void)v)

#define AX_IF_GET(a,b) ((a) ? (a) : (b))

#define AX_FUNC_NAME		axStrLiteral8(AX_FUNC_NAME_SZ)
#define AX_FUNC_FULLNAME	axStrLiteral8(AX_FUNC_FULLNAME_SZ)

#define ax_sizeof(a)	static_cast<axInt>(sizeof(a))
#define ax_alignof(a)	static_cast<axInt>(alignof(a))
#define	ax_move			::std::move
#define ax_forward(a)	::std::forward< decltype(a) >(a)

//#define axInitList		::std::initializer_list

#define axFunc			::std::function
//#define ax_bind			::std::bind

#define AX_RUN_ONCE(EXPR) \
	do{ \
		static bool _run_once_done = false; \
		if (!_run_once_done) { \
			_run_once_done = true; \
			{ EXPR; } \
		} \
	}while(false) \
//-------


#define AX_UNIQUE_NAME(name) 	AX_JOIN_WORD(axUNIQUE_NAME_##name, __LINE__)
#define AX_NO_NAME				AX_UNIQUE_NAME(no_name)

#define AX_TOGGLE_BOOL(v)	do{	v = !v; } while(false)

//#define ax_1			::std::placeholders::_1
//#define ax_2			::std::placeholders::_2
//#define ax_3			::std::placeholders::_3
//#define ax_4			::std::placeholders::_4
//#define ax_5			::std::placeholders::_5
//#define ax_6			::std::placeholders::_6
//#define ax_7			::std::placeholders::_7
//#define ax_8			::std::placeholders::_8
//#define ax_9			::std::placeholders::_9

#define AX_STATIC_ASSERT_MSG	static_assert
#define AX_STATIC_ASSERT(...)	AX_STATIC_ASSERT_MSG(__VA_ARGS__, #__VA_ARGS__)

#if AX_CPLUSPLUS_17
	#define AX_IF_CONSTEXPR if constexpr
#else
	#define AX_IF_CONSTEXPR if
#endif

#ifdef AX_GEN_CONFIG_Debug
	#define AX_DEBUG_ASSERT	AX_ASSERT
#else
	#define AX_DEBUG_ASSERT	do{ }while(false)
#endif

#define AX_DUMP_VAR_1(v0)				do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}]",									AX_STR(#v0), (v0)); } while(false)
#define AX_DUMP_VAR_2(v0, v1)			do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}]",							AX_STR(#v0), (v0), AX_STR(#v1), (v1)); } while(false)
#define AX_DUMP_VAR_3(v0, v1, v2)		do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}]",				AX_STR(#v0), (v0), AX_STR(#v1), (v1), AX_STR(#v2), (v2)); } while(false)
#define AX_DUMP_VAR_4(v0, v1, v2, v3)	do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}]",	AX_STR(#v0), (v0), AX_STR(#v1), (v1), AX_STR(#v2), (v2), AX_STR(#v3), (v3)); } while(false)

#define AX_DUMP_VAR_5(v0, v1, v2, v3, v4) \
	do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}]", \
		AX_STR(#v0), (v0), \
		AX_STR(#v1), (v1), \
		AX_STR(#v2), (v2), \
		AX_STR(#v3), (v3), \
		AX_STR(#v4), (v4) \
	); } while(false) \
//-----

#define AX_DUMP_VAR_6(v0, v1, v2, v3, v4, v5) \
	do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}]", \
		AX_STR(#v0), (v0), \
		AX_STR(#v1), (v1), \
		AX_STR(#v2), (v2), \
		AX_STR(#v3), (v3), \
		AX_STR(#v4), (v4), \
		AX_STR(#v5), (v5) \
	); } while(false) \
//-----

#define AX_DUMP_VAR_7(v0, v1, v2, v3, v4, v5, v6) \
	do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}]", \
		AX_STR(#v0), (v0), \
		AX_STR(#v1), (v1), \
		AX_STR(#v2), (v2), \
		AX_STR(#v3), (v3), \
		AX_STR(#v4), (v4), \
		AX_STR(#v5), (v5), \
		AX_STR(#v6), (v6) \
	); } while(false) \
//-----

#define AX_DUMP_VAR_8(v0, v1, v2, v3, v4, v5, v6, v7) \
	do{ AX_LOG("AX_DUMP_VAR: {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}], {?}=[{?}]", \
		AX_STR(#v0), (v0), \
		AX_STR(#v1), (v1), \
		AX_STR(#v2), (v2), \
		AX_STR(#v3), (v3), \
		AX_STR(#v4), (v4), \
		AX_STR(#v5), (v5), \
		AX_STR(#v6), (v6), \
		AX_STR(#v7), (v7) \
	); } while(false) \
//-----

#define AX_DUMP_VAR_SELECT(COUNT) AX_DUMP_VAR_##COUNT
#define AX_DUMP_VAR(...) AX_IDENTITY(AX_CALL(AX_DUMP_VAR_SELECT, AX_VA_ARGS_COUNT(__VA_ARGS__) (__VA_ARGS__)))

#define AX_DUMP_HEX(v)					do{ axLog::logHex(AX_LOC, axLog::Level::Info, axTempString::s_format("AX_DUMP_HEX: {?}", #v), v); } while(false)

#define AX_SE_NAMED_IO_1(se, v0)						do { se.named_io(#v0, v0); } while(false)
#define AX_SE_NAMED_IO_2(se, v0, v1)					do { se.named_io(#v0, v0); se.named_io(#v1, v1); } while(false)
#define AX_SE_NAMED_IO_3(se, v0, v1, v2)				do { se.named_io(#v0, v0); se.named_io(#v1, v1); se.named_io(#v2, v2); } while(false)
#define AX_SE_NAMED_IO_4(se, v0, v1, v2, v3)			do { se.named_io(#v0, v0); se.named_io(#v1, v1); se.named_io(#v2, v2); se.named_io(#v3, v3); } while(false)
#define AX_SE_NAMED_IO_5(se, v0, v1, v2, v3, v4)		do { se.named_io(#v0, v0); se.named_io(#v1, v1); se.named_io(#v2, v2); se.named_io(#v3, v3); se.named_io(#v4, v4); } while(false)
#define AX_SE_NAMED_IO_6(se, v0, v1, v2, v3, v4, v5)	do { se.named_io(#v0, v0); se.named_io(#v1, v1); se.named_io(#v2, v2); se.named_io(#v3, v3); se.named_io(#v4, v4); se.named_io(#v5, v5); } while(false)

#define AX_SE_NAMED_IO_7(se, v0, v1, v2, v3, v4, v5, v6) \
	do { se.named_io(#v0, v0); \
		 se.named_io(#v1, v1); \
		 se.named_io(#v2, v2); \
		 se.named_io(#v3, v3); \
		 se.named_io(#v4, v4); \
		 se.named_io(#v5, v5); \
		 se.named_io(#v6, v6); \
	} while(false)
//----

#define AX_SE_NAMED_IO_8(se, v0, v1, v2, v3, v4, v5, v6, v7) \
	do { se.named_io(#v0, v0); \
		 se.named_io(#v1, v1); \
		 se.named_io(#v2, v2); \
		 se.named_io(#v3, v3); \
		 se.named_io(#v4, v4); \
		 se.named_io(#v5, v5); \
		 se.named_io(#v6, v6); \
		 se.named_io(#v7, v7); \
	} while(false)
//----

#define AX_SE_NAMED_IO_SELECT(COUNT) AX_SE_NAMED_IO_##COUNT
#define AX_SE_NAMED_IO(se, ...) AX_IDENTITY(AX_CALL(AX_SE_NAMED_IO_SELECT, AX_VA_ARGS_COUNT(__VA_ARGS__) (se, __VA_ARGS__)))

#define AX_SE_NAMED_IO_PKEY_AUTO(se, V) do{ se.named_io_pkey(#V, V, true ); }while(false)
#define AX_SE_NAMED_IO_PKEY(     se, V) do{ se.named_io_pkey(#V, V, false); }while(false)

//------------

#define ax_se_io_1(se, v0)						do { se.io(v0); } while(false)
#define ax_se_io_2(se, v0, v1)					do { se.io(v0); se.io(v1); } while(false)
#define ax_se_io_3(se, v0, v1, v2)				do { se.io(v0); se.io(v1); se.io(v2); } while(false)
#define ax_se_io_4(se, v0, v1, v2, v3)			do { se.io(v0); se.io(v1); se.io(v2); se.io(v3); } while(false)
#define ax_se_io_5(se, v0, v1, v2, v3, v4)		do { se.io(v0); se.io(v1); se.io(v2); se.io(v3); se.io(v4); } while(false)
#define ax_se_io_6(se, v0, v1, v2, v3, v4, v5)	do { se.io(v0); se.io(v1); se.io(v2); se.io(v3); se.io(v4); se.io(v5); } while(false)

#define ax_se_io_7(se, v0, v1, v2, v3, v4, v5, v6) \
	do { se.io(v0); \
		 se.io(v1); \
		 se.io(v2); \
		 se.io(v3); \
		 se.io(v4); \
		 se.io(v5); \
		 se.io(v6); \
	} while(false)
//----

#define ax_se_io_8(se, v0, v1, v2, v3, v4, v5, v6, v7) \
	do { se.io(v0); \
		 se.io(v1); \
		 se.io(v2); \
		 se.io(v3); \
		 se.io(v4); \
		 se.io(v5); \
		 se.io(v6); \
		 se.io(v7); \
	} while(false)
//----

#define ax_se_io_SELECT(COUNT) ax_se_io_##COUNT
#define AX_SE_IO(se, ...) AX_IDENTITY(AX_CALL(ax_se_io_SELECT, AX_VA_ARGS_COUNT(__VA_ARGS__) (se, __VA_ARGS__)))
