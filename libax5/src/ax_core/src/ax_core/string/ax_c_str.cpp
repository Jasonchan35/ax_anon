//
//  axCStr_parse.cpp
//  ax_core
//
//  Created by Jason Chan on 2012-09-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <ax_core/string/axString.h>

//==================  axCStr_parse char ============================
template<> 
bool axCStr_Impl<const char>::parse(const char* sz, bool &v) {
	if (caseEquals(sz, "true" )) { v = true;  return true; }
	if (caseEquals(sz, "false")) { v = false; return true; }
	return false;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, uint8_t    &v ) {
	uint32_t tmp;
	if( 1 != sscanf( sz, "%u", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, uint16_t    &v ) {
	uint32_t tmp;
	if( 1 != sscanf( sz, "%u", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, uint32_t   &v ) {
	if( 1 != sscanf( sz, "%u", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, uint64_t   &v ) {
	if( 1 != sscanf( sz, "%llu", (long long unsigned*)&v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, int8_t    &v ) {
	int32_t tmp;
	if( 1 != sscanf( sz, "%d", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, int16_t    &v ) {
	int32_t tmp;
	if( 1 != sscanf( sz, "%d", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, int32_t   &v ) {
	if( 1 != sscanf( sz, "%d", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, int64_t   &v ) {
	if( 1 != sscanf( sz, "%lld", (long long int *)&v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, axFloat32  &v ) {
	if( 1 != sscanf( sz, "%f", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, axFloat64 &v ) {
	if( 1 != sscanf( sz, "%lf", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const char>::parse( const char* sz, axFloat128 &v ) {
	if( 1 != sscanf( sz, "%Lf", &v ) )
		return false;
	return true;
}

//==================  axCStr_parse wchar ============================
template<> 
bool axCStr_Impl<const wchar_t>::parse(const wchar_t* sz, bool &v) {
	if (caseEquals(sz, L"true" )) { v = true;  return true; }
	if (caseEquals(sz, L"false")) { v = false; return true; }
	return false;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, uint8_t    &v ) {
	uint32_t tmp;
	if( 1 != swscanf( sz, L"%u", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, uint16_t    &v ) {
	uint32_t tmp;
	if( 1 != swscanf( sz, L"%u", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, uint32_t   &v ) {
	if( 1 != swscanf( sz, L"%u", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, uint64_t   &v ) {
	if( 1 != swscanf( sz, L"%llu", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, int8_t    &v ) {
	int32_t tmp;
	if( 1 != swscanf( sz, L"%d", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, int16_t    &v ) {
	int32_t tmp;
	if( 1 != swscanf( sz, L"%d", &tmp ) )
		return false;
	ax_safe_assign( v, tmp );
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, int32_t   &v ) {
	if( 1 != swscanf( sz, L"%d", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, int64_t   &v ) {
	if( 1 != swscanf( sz, L"%lld", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, axFloat32  &v ) {
	if( 1 != swscanf( sz, L"%f", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, axFloat64 &v ) {
	if( 1 != swscanf( sz, L"%lf", &v ) )
		return false;
	return true;
}

template<>
bool axCStr_Impl<const wchar_t>::parse( const wchar_t* sz, axFloat128 &v ) {
	if( 1 != swscanf( sz, L"%Lf", &v ) )
		return false;
	return true;
}

//==================  axCStr_parse axChar16 / axChar32 ============================
template<class CHAR, class VAL>
bool axCStr_parse_impl( const CHAR* sz, VAL& outValue ) {
	axTempString8	buf;
	buf.setUtf_c_str(sz);
	return axCStr_Impl<const axChar8>::parse(buf.c_str(), outValue);
}

#define AX_MACRO_OP(VAL) \
	template<> bool axCStr_Impl<const axChar16>::parse( const axChar16* sz, VAL& outValue ) { return axCStr_parse_impl(sz, outValue); } \
	template<> bool axCStr_Impl<const axChar32>::parse( const axChar32* sz, VAL& outValue ) { return axCStr_parse_impl(sz, outValue); } \
//-----------
	AX_MACRO_OP(bool)
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP

//=============================

template<class T>
bool axCStr_Impl<T>::hasPrefix(const T* full, const T* prefix) {
	if (!full || !prefix) return false;
	axInt prefix_len = length(prefix);
	return equals(prefix, full, prefix_len);
}

template<class T>
bool axCStr_Impl<T>::hasSuffix(const T* full, const T* suffix) {
	if (!full || !suffix) return false;

	axInt	suffix_len = length(suffix);
	axInt	full_len = length(full);

	if (full_len < suffix_len) return false;
	axInt n = full_len - suffix_len;
	const T* p = full + n;
	return equals(suffix, p, suffix_len);
}

template<class T>
T* axCStr_Impl<T>::findBackStr(T* a, T* b) {
	if (!a || !b) return nullptr;

	axInt alen, blen;

	T *p;

	alen = length(a);
	blen = length(b);

	if (blen > alen) return nullptr;

	for (p = a + alen - blen; p >= a; p--) {
		if (equals(p, b, blen)) {
			return p;
		}
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findChar(T* sz, T ch) {
	if (!sz) return nullptr;

	for (; *sz; sz++) {
		if (*sz == ch) return sz;
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findStr(T* a, T* b) {
	if (!a || !b) return nullptr;

	T* sa = a;
	T* sb = b;
	if (*b == 0) return nullptr;
	for (; *a; a++) {
		sa = a;
		sb = b;
		for (;;) {
			if (*sb == 0) return a; //found
			if (*sb != *sa) break;
			sa++; sb++;
		}
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findCharBack(T* sz, T ch) {
	if (!sz) return nullptr;

	axInt len = length(sz);
	if (len == 0) return nullptr;
	T *s = &sz[len - 1];
	for (; s >= sz; s--) {
		if (*s == ch) return s;
	}
	return nullptr;
}


template<class T>
axInt axCStr_Impl<T>::compare( const T* s1, const T* s2 ) {
		if( s1 == s2 ) return 0;
		if( !s1 ) return -1;
		if( !s2 ) return  1;

		for( ;; s1++, s2++ ) {
			if( *s1 != *s2 ) return (*s1-*s2);
			if( *s1 == 0 || *s2 == 0 ) break;
		}
		return 0;
	}

template<class T>
axInt axCStr_Impl<T>::compare( const T* s1, const T* s2, axInt n ) {
	if( s1 == s2 ) return 0;
	if( !s1 ) return -1;
	if( !s2 ) return  1;

	axInt i;
	for( i=0; i<n; s1++, s2++, i++ ) {
		if( *s1 != *s2 ) return (*s1-*s2);
		if( *s1 == 0 || *s2 == 0 ) break;
	}
	return 0;
}

template<class T>
axInt axCStr_Impl<T>::caseCompare(const T* s1, const T* s2) {
	if( s1 == s2 ) return 0;
	if( ! s1 ) return -1;
	if( ! s2 ) return  1;

	for( ;; s1++, s2++ ) {
		T c1 = toUpper(*s1);
		T c2 = toUpper(*s2);
		if( c1 != c2 ) return (c1-c2);
		if( c1 == 0 || c2 == 0 ) break;
	}
	return 0;
}

template<class T>
axInt axCStr_Impl<T>::caseCompare(const T* s1, const T* s2, axInt n) {
	if( s1 == s2 ) return 0;
	if( ! s1 ) return -1;
	if( ! s2 ) return  1;

	axInt i;
	for( i=0; i<n; s1++, s2++, i++ ) {
		if (auto d = charCaseCompare(*s1, *s2)) {
			return d;
		}
		if( *s1 == 0 || *s2 == 0 ) break;
	}
	return 0;
}

template<class T>
T* axCStr_Impl<T>::findChars(T* sz, const T* chrs) {
	if (!sz || !chrs) return nullptr;

	T* p = sz;
	for (; *p; p++) {
		for (const T* c = chrs; *c; c++) {
			if (*p == *c) return p;
		}
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findCharsBack(T* sz, const T* chrs) {
	if (!*sz || !chrs) return nullptr;

	axInt len = length(sz);
	T* p = sz + len - 1;
	for (; p >= sz; p--) {
		for (const T* c = chrs; *c; c++) {
			if (*p == *c) return p;
		}
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findCaseStr(T* big, const T* little) {
	if (!big || !little) return nullptr;

	axInt big_len = length(big);
	axInt little_len = length(little);

	if (little_len > big_len) return nullptr;

	axInt n = big_len - little_len;
	axInt i;
	for (i = 0; i <= n; i++) {
		if (caseCompare(big + i, little, little_len) == 0)
			return big + i;
	}
	return nullptr;
}

template<class T>
T* axCStr_Impl<T>::findCaseChar(T* sz, T ch) {
	if (!sz) return nullptr;

	T c = toUpper(ch);
	for (; *sz; sz++) {
		if (toUpper(*sz) == c) return sz;
	}
	return nullptr;
}

template<class T>
axHash axCStr_Impl<T>::getHash(const T *sz) {
	using Value = axHash::Value;
	if (!sz) return axHash(877);
	/*
	"djb2" string hash
	this algorithm (k=33) was first reported by dan bernstein many years ago
	in comp.lang.c. another version of this algorithm (now favored by bernstein)
	uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33
	(why it works better than many other constants, prime or not) has never
	been adequately explained.
	*/
	Value h(5381);
	for (; *sz; sz++) {
		h = ((h << 5) + h) + static_cast<Value>(*sz); // h * 33 + c
	}
	return axHash(h);
}

//==============================

#define AX_MACRO_OP(T) \
	template class axCStr_Impl<const T>; \
	template class axCStrMut_<T>; \
//----
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP
