//
//  ax_atomic.h
//  ax_core
//
//  Created by Jason on 2015-03-20.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

class axAtomicFlag : public axNonCopyable {
	typedef ::std::atomic_flag V;
public:
	AX_INLINE bool testAndSet()	{ return _v.test_and_set(); }
	AX_INLINE void clear()			{ _v.clear(); }
private:
	V _v = ATOMIC_FLAG_INIT;
};


template< class T >
class axAtomicT : public axNonCopyable {
	typedef	std::atomic<T> Atomic;
public:
	AX_INLINE axAtomicT(T v = T()) : _v(v) {}

	AX_INLINE  void	set			( const T& v )  { _v.store(v); }
	AX_INLINE	T		get 		() const		{ return _v.load(); }

	AX_INLINE	void	operator=	( const T& v )	{ set(v); }
	AX_INLINE			operator T	() const		{ return get(); }

	AX_INLINE	T		operator++	() 				{ return ++_v; }
	AX_INLINE	T		operator--	()				{ return --_v; }

private:
	::std::atomic<T>	_v;
};

using axAtomicInt		= axAtomicT<axInt>;
using axAtomicInt8		= axAtomicT<axInt8>;
using axAtomicInt16		= axAtomicT<axInt16>;
using axAtomicInt32		= axAtomicT<axInt32>;
using axAtomicInt64		= axAtomicT<axInt64>;

using axAtomicUInt		= axAtomicT<axUInt>;
using axAtomicUInt8		= axAtomicT<axUInt8>;
using axAtomicUInt16	= axAtomicT<axUInt16>;
using axAtomicUInt32	= axAtomicT<axUInt32>;
using axAtomicUInt64	= axAtomicT<axUInt64>;
