//
//  ax_c_str.h
//  ax_core
//
//  Created by Jason Chan on 3/16/15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

#include <ax_core/base/ax_base.h>
#include <ax_core/base/axHash.h>

template<class T>
class axCStr_Impl {
public:
	static const T*	kEmpty			();
	static const T* kPathSeparators	();
	static const T* kHexChars		();
	static const T* kLowHexChars	();

	static bool isHex(T ch) {
		if( ch >= '0' && ch <='9' ) return true;
		if( ch >= 'A' && ch <='F' ) return true;
		if( ch >= 'a' && ch <='f' ) return true;
		return false;
	}

	static axUInt8 hexToInt(T ch) {
		if( ch >= '0' && ch <='9' ) return static_cast<axUInt8>(ch - '0');
		if( ch >= 'a' && ch <='f' ) return static_cast<axUInt8>(ch - 'a' + 10);
		if( ch >= 'A' && ch <='F' ) return static_cast<axUInt8>(ch - 'A' + 10);
		throw axError_Undefined(AX_LOC);
	}

	static T escapeChar(T c) {
		switch( c ) {
			case 'n': return '\n';
			case 'r': return '\r';
			case 'b': return '\b';
			case 't': return '\t';
		}
		return c;
	}

	static bool	isUpper(T ch) { return ( ch>='A' && ch <= 'Z' ); }
	static bool	isLower(T ch) { return ( ch>='a' && ch <= 'z' ); }

	static bool	isAlpha(T ch) { return isUpper(ch) || isLower(ch); }
	static bool	isDigit(T ch) { return ( ch >= '0' && ch <='9' ); }

	static T	toUpper(T ch) { return isLower(ch) ? (ch - ('a' - 'A')) : ch; }
	static T	toLower(T ch) { return isUpper(ch) ? (ch + ('a' - 'A')) : ch; }

	static axInt length( const T* s ) {
		if( !s ) return 0;

		axInt len = 0;
		for( ; *s; s++, len++ ) {
		}
		return len;
	}

	static axInt length(const T* s, axInt maxLen) {
		if (!s) return 0;
		axInt l=0;
		for (;;) {
			if (l > maxLen) return maxLen;
			if (*s == 0) return l;
			l++;
			s++;
		}
	}

	static axInt charCaseCompare(T a, T b) { return toUpper(a) - toUpper(b); }
	static bool  charCaseEquals (T a, T b) { return charCaseCompare(a, b) == 0; }

	static axInt caseCompare(const T* s1, const T* s2);
	static axInt caseCompare(const T* s1, const T* s2, axInt n);

	static bool equals(const T* s1, const T* s2)				{ return compare(s1, s2) == 0; }
	static bool equals(const T* s1, const T* s2, axInt n)		{ return compare(s1, s2, n) == 0; }

	static bool caseEquals(const T* s1, const T* s2)			{ return caseCompare(s1, s2) == 0; }
	static bool caseEquals(const T* s1, const T* s2, axInt n)	{ return caseCompare(s1, s2, n) == 0; }

	static bool	hasPrefix( const T* full, const T* prefix );
	static bool	hasSuffix( const T* full, const T* suffix );

	static T*	findChars		(T* sz, const T* chrs);
	static T*	findCharsBack	(T* sz, const T* chrs);
	static T*	findCaseChar		(T* sz, T ch);
	static T*	findCaseStr			(T* big, const T* little );

	static T*	findChar			( T* sz, T ch );
	static T*	findCharBack		( T* sz, T ch );
	static T*	findStr				( T* a, T* b );
	static T*	findBackStr			( T* a, T* b );

	static axInt compare	( const T* s1, const T* s2 );
	static axInt compare	( const T* s1, const T* s2, axInt n );

	static axHash getHash(const T *sz);

	static T* fileBasename(T* sz) {
		auto p = findCharsBack(sz, kPathSeparators());
		return p ? p+1 : sz;
	}

	static bool parse(const T* sz, bool &v);

	static bool parse(const T* sz, axInt8  &v);
	static bool parse(const T* sz, axInt16 &v);
	static bool parse(const T* sz, axInt32 &v);
	static bool parse(const T* sz, axInt64 &v);

	static bool parse(const T* sz, axUInt8  &v);
	static bool parse(const T* sz, axUInt16 &v);
	static bool parse(const T* sz, axUInt32 &v);
	static bool parse(const T* sz, axUInt64 &v);

	static bool parse(const T* sz, axFloat32  &v);
	static bool parse(const T* sz, axFloat64  &v);
	static bool parse(const T* sz, axFloat128 &v);
};

template<class T> using axCStr_ = axCStr_Impl<const T>;

#define AX_MACRO_OP(T) \
	template<> inline const axChar##T* axCStr_Impl<const axChar##T>::kEmpty			() { return AX_SZ_##T( ""); } \
	template<> inline const axChar##T* axCStr_Impl<const axChar##T>::kPathSeparators() { return AX_SZ_##T("\\/"); } \
	template<> inline const axChar##T* axCStr_Impl<const axChar##T>::kHexChars		() { return AX_SZ_##T("0123456789ABCDEF"); } \
	template<> inline const axChar##T* axCStr_Impl<const axChar##T>::kLowHexChars	() { return AX_SZ_##T("0123456789abcdef"); } \
//------
	AX_MACRO_OP(8)
	AX_MACRO_OP(16)
	AX_MACRO_OP(32)
	AX_MACRO_OP(W)
#undef AX_MACRO_OP

template<class T>
class axCStrMut_ {
public:
	static T* copy(T* dst, const T* src, axInt n) {
		if( !dst || !src ) return nullptr;

		axInt i;
		T* ret = dst;
		for( i=0; i<n; i++ ) {
			if( *src == 0 ) break;
			*dst = *src;
			dst++; src++;
		}
		*dst = 0;
		return ret;
	}

	static T* copy( T* dst, const T* src ) {
		if( !dst || !src ) return nullptr;

		T* ret = dst;
		for(;;) {
			if( *src == 0 ) break;
			*dst = *src;
			dst++; src++;
		}
		*dst = 0;
		return ret;
	}
};

class axCStr {
public:
	template<class T = axChar> AX_INLINE static const T* kEmpty			() { return axCStr_<T>::kEmpty(); }
	template<class T = axChar> AX_INLINE static const T* kPathSeparators() { return axCStr_<T>::kPathSeparators(); }
	template<class T = axChar> AX_INLINE static const T* kHexChars		() { return axCStr_<T>::kHexChars(); }
	template<class T = axChar> AX_INLINE static const T* kLowHexChars	() { return axCStr_<T>::kLowHexChars(); }

	template<class T> AX_INLINE static axInt	length(const T* sz)					{ return axCStr_<T>::length(sz); }
	template<class T> AX_INLINE static axInt	length(const T* sz, axInt maxLen)	{ return axCStr_<T>::length(sz, maxLen); }

	template<class T> AX_INLINE static bool		isUpper(T ch) { return axCStr_<T>::isUpper(ch); }
	template<class T> AX_INLINE static bool		isLower(T ch) { return axCStr_<T>::isLower(ch); }
						 				
	template<class T> AX_INLINE static bool		isAlpha(T ch) { return axCStr_<T>::isAlpha(ch); }
	template<class T> AX_INLINE static bool		isDigit(T ch) { return axCStr_<T>::isDigit(ch); }
	
	template<class T> AX_INLINE static axUInt8	hexToInt(T ch) { return axCStr_<T>::hexToInt(ch); }

	template<class T> AX_INLINE static T		toUpper(T ch) { return axCStr_<T>::toUpper(ch); }
	template<class T> AX_INLINE static T		toLower(T ch) { return axCStr_<T>::toLower(ch); }

	template<class T> AX_INLINE static axInt	charCaseCompare(T a, T b) { return axCStr_<T>::charCaseCompare(a, b); }
	template<class T> AX_INLINE static bool		charCaseEquals (T a, T b) { return axCStr_<T>::charCaseCompare(a, b) == 0; }

	template<class T> AX_INLINE static axInt	compare		(const T* s1, const T* s2)			{ return axCStr_<T>::compare(s1, s2); }
	template<class T> AX_INLINE static axInt	compare		(const T* s1, const T* s2, axInt n)	{ return axCStr_<T>::compare(s1, s2, n); }

	template<class T> AX_INLINE static axInt	caseCompare	(const T* s1, const T* s2)			{ return axCStr_<T>::caseCompare(s1, s2); }
	template<class T> AX_INLINE static axInt	caseCompare	(const T* s1, const T* s2, axInt n)	{ return axCStr_<T>::caseCompare(s1, s2, n); }

	template<class T> AX_INLINE static bool		equals		(const T* s1, const T* s2)			{ return axCStr_<T>::equals(s1, s2); }
	template<class T> AX_INLINE static bool		equals		(const T* s1, const T* s2, axInt n)	{ return axCStr_<T>::equals(s1, s2, n); }

	template<class T> AX_INLINE static bool		caseEquals	(const T* s1, const T* s2)			{ return axCStr_<T>::caseEquals(s1, s2); }
	template<class T> AX_INLINE static bool		caseEquals	(const T* s1, const T* s2, axInt n)	{ return axCStr_<T>::caseEquals(s1, s2, n); }

	template<class T> AX_INLINE static bool		hasPrefix	(const T* s1, const T* s2)		{ return axCStr_<T>::hasPrefix(s1, s2); }
	template<class T> AX_INLINE static bool		hasSuffix	(const T* s1, const T* s2)		{ return axCStr_<T>::hasSuffix(s1, s2); }

	template<class T> AX_INLINE static T*		findChar		(T* s1, T ch)				{ return axCStr_<T>::findChar		(s1, ch); }
	template<class T> AX_INLINE static T*		findCaseChar	(T* s1, T ch)				{ return axCStr_<T>::findCaseChar	(s1, ch); }
	template<class T> AX_INLINE static T*		findChars		(T* s1, const T* chs)		{ return axCStr_<T>::findChars	(s1, chs); }
	template<class T> AX_INLINE static T*		findStr			(T* s1, const T* s2)		{ return axCStr_<T>::findStr		(s1, s2); }
	template<class T> AX_INLINE static T*		findCaseStr		(T* s1, const T* s2)		{ return axCStr_<T>::findCaseStr	(s1, s2); }

	template<class T> AX_INLINE static T*		findEndChar		(T* s1, T ch)				{ return axCStr_<T>::findChar		(s1, ch); }
	template<class T> AX_INLINE static T*		findEndCaseChar	(T* s1, T ch)				{ return axCStr_<T>::findCaseChar	(s1, ch); }
	template<class T> AX_INLINE static T*		findEndMultiChars	(T* s1, const T* chs)		{ return axCStr_<T>::findChars	(s1, chs); }
	template<class T> AX_INLINE static T*		findEndStr		(T* s1, const T* s2)		{ return axCStr_<T>::findStr		(s1, s2); }
	template<class T> AX_INLINE static T*		findEndCaseStr	(T* s1, const T* s2)		{ return axCStr_<T>::findCaseStr	(s1, s2); }

	template<class T> AX_INLINE static T*		fileBasename	(T* sz)						{ return axCStr_<T>::fileBasename(sz); }

	template<class T> AX_INLINE static T*		getHash(const T* s1)						{ return axCStr_<T>::getHash(s1); }

	template<class T> AX_INLINE static bool		parse(const T* sz, bool       &v)			{ return axCStr_<T>::parse(sz, v); }
					 	 				
	template<class T> AX_INLINE static bool		parse(const T* sz, axInt8     &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axInt16    &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axInt32    &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axInt64    &v)			{ return axCStr_<T>::parse(sz, v); }

	template<class T> AX_INLINE static bool		parse(const T* sz, axUInt8    &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axUInt16   &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axUInt32   &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axUInt64   &v)			{ return axCStr_<T>::parse(sz, v); }

	template<class T> AX_INLINE static bool		parse(const T* sz, axFloat32  &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axFloat64  &v)			{ return axCStr_<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, axFloat128 &v)			{ return axCStr_<T>::parse(sz, v); }

	template<class T> AX_INLINE static T*		copy	(T* s1, const T* s2)				{ return axCStrMut_<T>::copy(s1, s2); }
	template<class T> AX_INLINE static T*		copy	(T* s1, const T* s2, axInt n)		{ return axCStrMut_<T>::copy(s1, s2, n); }
};

//template<class T> axInt ax_strlen (const T* sz)			{ return axCStr::length(sz); }
//template<class T> axInt ax_strnlen(const T* sz, axInt n)	{ return axCStr::length(sz, n); }

