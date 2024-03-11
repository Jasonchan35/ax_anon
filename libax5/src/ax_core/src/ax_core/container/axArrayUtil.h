//
//  ArrayUtil.h
//  ax_core
//
//  Created by Jason on 2015-03-15.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

#include "../allocator/axAllocator.h"
#include "../base/axTypeTraits.h"
#include "../base/axTag.h"

template<class T> class axMutSpan;
template<class T> using axSpan = axMutSpan<const T>;

struct axArrayUtil : axStaticClass {
	static const axInt k_size_to_use_system_memcpy = 2048;

	template< class A, class B >
	AX_INLINE static bool isOverlapped(const A* a, axInt a_size, const B* b, axInt b_size);

	template< class T >
	AX_INLINE static bool equals( const T* a, const T* b, axInt n );

	template< class T >
	AX_INLINE static void copy( T* dst, const T* src, axInt n );

	template< class DST, class SRC >
	AX_INLINE static void constCast( DST* dst, const SRC* src, axInt n );

	template< class DST, class SRC >
	AX_INLINE static void staticCast( DST* dst, const SRC* src, axInt n );

	template< class DST, class SRC >
	AX_INLINE static void reinterpretCast( DST* dst, const SRC* src, axInt n );

	template< class T, class... Args >
	AX_INLINE static void constructor( T* p, axInt n, Args&&... args );

	template< class T >
	AX_INLINE static void constructor( T* p, axInt n, axTag::NoInitT);

	template< class T >
	AX_INLINE static void copyConstructor( T* dst, const T* src, axInt n );

	template< class T >
	AX_INLINE static void moveConstructor( T* dst, T* src, axInt n );

	template< class T >
	AX_INLINE static void moveConstructorAndDestructor( T* dst, T* src, axInt n );

	template< class T >
	AX_INLINE static void destructor( T* p, axInt n );

	AX_INLINE static void  internal_memcpy( void* dst, const void* src, axInt len );
	AX_INLINE static void  internal_memset( void* dst, axByte value, axInt len );
	AX_INLINE static axInt internal_memcmp( const void* a, const void* b, axInt len );
	AX_INLINE static void  internal_bzero ( void* dst, axInt len );

	//only works for data type without padding
	AX_INLINE static bool internal_mem_equals( const void* a, const void* b, axInt len );

	template< class T >
	AX_INLINE static void internal_copy_loop( T* dst, const T* src, axInt len );

	template< class T >
	AX_INLINE static void internal_fill_loop( T* dst, const T& value, axInt len );

	template< class T >
	AX_INLINE static bool internal_equals_loop( const T* dst, const T* src, axInt len );
};

AX_INLINE void	 ax_memcpy(void* dst, const void* src, axInt len)			{ axArrayUtil::internal_memcpy(dst, src, len); }
AX_INLINE void	 ax_memset(void* dst, axByte value, axInt len)				{ axArrayUtil::internal_memset(dst, value, len); }
AX_INLINE axInt ax_memcmp(const void* a, const void* b, axInt len)			{ return axArrayUtil::internal_memcmp(a, b, len); }
AX_INLINE void  ax_bzero (void* dst, axInt len)							{ axArrayUtil::internal_bzero(dst, len); }

template<class T>
AX_INLINE void ax_bzero_struct(T& v) {
	AX_STATIC_ASSERT(!axTypeTraits::isPointer<T>::value);
	ax_bzero(&v, ax_sizeof(v));
}

//--------- inline -----------
void axArrayUtil::internal_memcpy( void* dst, const void* src, axInt len ) {
	if( len <= 0 ) return;

	if (isOverlapped((const char*)dst, len, (const char*)src, len)) {
		throw axError_BufferOverlapped();
	}

	if( len > k_size_to_use_system_memcpy ) {
		::memcpy( dst, src, axInt_to_size_t(len) );
		return;
	}
	const axInt w = ax_sizeof( size_t );
	auto n = len / w;
	auto r = len % w;
	auto j = n*w;
	internal_copy_loop( reinterpret_cast<size_t*>(dst),   reinterpret_cast<const size_t*>(src),   n );
	internal_copy_loop( reinterpret_cast<axByte*>(dst)+j, reinterpret_cast<const axByte*>(src)+j, r );
}

axInt axArrayUtil::internal_memcmp(const void* a, const void* b, axInt len ) {
	if( len <= 0 ) return 0;
	if( len > k_size_to_use_system_memcpy ) {
		AX_STATIC_ASSERT(sizeof(axInt) >= sizeof(int));
		return ::memcmp(a, b, axInt_to_size_t(len));
	}

	auto* d = reinterpret_cast<const uint8_t*>(a);
	auto* s = reinterpret_cast<const uint8_t*>(b);
	auto* e = s + len;
	for (; s < e; s++, d++) {
		if (*d != *s)
			return axInt(*d) - axInt(*s);
	}
	return 0;
}

void axArrayUtil::internal_memset( void* dst, axByte value, axInt len ) {
	if( len <= 0 ) return;
	if( len > k_size_to_use_system_memcpy ) {
		::memset( dst, value, axInt_to_size_t( len ) );
		return;
	}

#if AX_SIZEOF_POINTER == 8
	size_t batch = (size_t)value | (size_t)value<<8 | (size_t)value<<16 | (size_t)value<<24 | (size_t)value<<32 | (size_t)value<<40 | (size_t)value<<48 | (size_t)value<<56;
#elif AX_SIZEOF_POINTER == 4
	size_t batch = (size_t)value | (size_t)value<<8 | (size_t)value<<16 | (size_t)value<<24;
#else
	#error
#endif

	const axInt w = ax_sizeof( size_t );
	auto n = len / w;
	auto r = len % w;
	auto j = n*w;
	internal_fill_loop( reinterpret_cast<size_t*>(dst),   batch, n );
	internal_fill_loop( reinterpret_cast<axByte*>(dst)+j, value, r );
}

template< class T >
void axArrayUtil::internal_copy_loop( T* dst, const T* src, axInt len ) {
	auto* e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		*dst = *src;
	}
}

template< class T >
void axArrayUtil::internal_fill_loop( T* dst, const T& value, axInt len ) {
	auto e = dst + len;
	for( ; dst < e; ++dst ) {
		*dst = value;
	}
}

void axArrayUtil::internal_bzero( void* dst, axInt len ) {
	if( len <= 0 ) return;
	if( len > k_size_to_use_system_memcpy ) {
		::memset( dst, 0, axInt_to_size_t(len) );
		return;
	}
	const axInt w = ax_sizeof( size_t );
	auto n = len / w;
	auto r = len % w;
	auto j = n*w;
	internal_fill_loop( reinterpret_cast<size_t*>(dst),   (size_t)0, n );
	internal_fill_loop( reinterpret_cast<axByte*>(dst)+j, (axByte)0, r );
}

template< class T >
bool axArrayUtil::internal_equals_loop( const T* dst, const T* src, axInt len ) {
	auto e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		if (*dst != *src) return false;
	}
	return true;
}

template< class DST, class SRC > AX_INLINE
void	axArrayUtil::constCast( DST* dst, const SRC* src, axInt n ) {
	auto e = src + n;
	for( ; src<e; ++src, ++dst ) {
		*dst = const_cast<DST>(*src);
	}
}

template< class DST, class SRC > AX_INLINE 
void	axArrayUtil::staticCast( DST* dst, const SRC* src, axInt n ) {
	auto e = src + n;
	for( ; src<e; ++src, ++dst ) {
		*dst = static_cast<DST>(*src);
	}
}

template< class DST, class SRC > AX_INLINE 
void	axArrayUtil::reinterpretCast( DST* dst, const SRC* src, axInt n ) {
	auto e = src + n;
	for( ; src<e; ++src, ++dst ) {
		*dst = reinterpret_cast<DST>(*src);
	}
}

template< class T, class... Args > AX_INLINE
void axArrayUtil::constructor( T* p, axInt n, Args&&... args ) {
	if( n <= 0 ) return;

	auto d = p;
	auto e = p + n;
    for( ; d<e; ++d ) {
		ax_call_constructor(d, ax_forward(args)...);
    }
}

template< class T > AX_INLINE
void axArrayUtil::constructor( T* p, axInt n, axTag::NoInitT) {
	AX_IF_CONSTEXPR(axTypeTraits::isPod<T>::value) {
		/* no init */
	} else {
		constructor(p, n);
	}
}

template< class T > AX_INLINE
void axArrayUtil::copyConstructor( T* dst, const T* src, axInt n ) {
	if( n <= 0 ) return;

	if( axTypeTraits::isTriviallyCopyable<T>::value ) {
		ax_memcpy( dst, src, n * ax_sizeof(T) );
	}else{
		auto s = src;
		auto e = src + n ;
        for( ; s<e; ++s, ++dst ) {
            ax_call_constructor(dst, *s);
        }
	}
}

template< class T > AX_INLINE
void axArrayUtil::moveConstructor( T* dst, T* src, axInt n ) {
	if( n <= 0 ) return;

	if( axTypeTraits::isTriviallyCopyable<T>::value ) {
		ax_memcpy( dst, src, n * ax_sizeof(T) );
	}else{
		auto s = src;
		auto e = src + n ;
        for( ; s<e; ++s, ++dst ) {
            ax_call_constructor(dst, ax_move(*s));
        }
	}
}

template< class T > AX_INLINE
void axArrayUtil::moveConstructorAndDestructor( T* dst, T* src, axInt n ) {
	if( n <= 0 ) return;

	if( axTypeTraits::isTriviallyCopyable<T>::value ) {
		ax_memcpy( dst, src, n * ax_sizeof(T) );
	}else{
		auto s = src;
		auto e = src + n ;
        for( ; s<e; ++s, ++dst ) {
            ax_call_constructor(dst, ax_move(*s));
			ax_call_destructor(s);
        }
	}
}

template< class T > AX_INLINE
void axArrayUtil::destructor( T* p, axInt n ) {
	if( axTypeTraits::isPod<T>::value ) return;

	if( n <= 0 ) return;

	T *e = p + n;
	for( ; p<e; ++p ) {
		ax_call_destructor(p);
	}
}

template< class T > AX_INLINE
void axArrayUtil::copy(T* dst, const T* src, axInt n) {
	if (n <= 0) return;

	if (isOverlapped(dst, n, src, n)) {
		throw axError_BufferOverlapped();
	}

	if (axTypeTraits::isTriviallyCopyable<T>::value) {
		ax_memcpy(dst, src, n * ax_sizeof(T));
	} else {
		internal_copy_loop(dst, src, n);
	}
}

template< class T > AX_INLINE
bool axArrayUtil::equals(const T* a, const T* b, axInt n) {
	if (n <= 0) return true;
	if (a == b) return true;

	if (axTypeTraits::isTriviallyComparable<T>::value) {
		return internal_mem_equals(a, b, n * ax_sizeof(T));
	} else {
		return internal_equals_loop(a, b, n);
	}
}

AX_INLINE
bool axArrayUtil::internal_mem_equals(const void* a, const void* b, axInt len) {
	if( len <= 0 ) return true;
	if( len > k_size_to_use_system_memcpy ) {
		return 0 == ::memcmp( a, b, axInt_to_size_t(len) );
	}
	const axInt w = ax_sizeof( size_t );
	auto n = len / w;
	auto r = len % w;
	auto j = n*w;
	if (!internal_equals_loop( (size_t*)a,   (size_t*)b,   n )) return false;
	if (!internal_equals_loop( (axByte*)a+j, (axByte*)b+j, r )) return false;
	return true;
}

template< class A, class B >
bool axArrayUtil::isOverlapped(const A* a, axInt a_size, const B* b, axInt b_size) { 
	const void* ea = a + a_size;
	const void* eb = b + b_size; 
	return (ea > b) && (eb > a);
}
