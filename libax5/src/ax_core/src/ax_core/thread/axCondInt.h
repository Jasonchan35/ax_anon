//
//  axCondInt.h
//  libax_core
//
//  Created by Jason on 2018-06-16.
//
//

#pragma once

#include "axCondMutex.h"

template<class T, bool bBroadcast>
class axCondIntT {
	using This = axCondIntT;
public:
	using Type = T;

	struct MData {
		T value = 0;
	};
	using ProtectedData = axCondMutexProtected<MData, bBroadcast>;
	using ScopedLock = typename ProtectedData::ScopedLock;

	axCondIntT() = default;
	axCondIntT(T v)	{ set(v); }
	
	AX_INLINE	T	get () const	{
		auto m = ax_const_cast(this)->scopedLock();
		return m->value;
	}
	AX_INLINE		operator T	() const	{ return get(); }

	AX_INLINE	T	set	(T newValue)		{ return setByFunc([=](T v) {return newValue;}); }
	AX_INLINE	T	operator=(T newValue)	{ return set(newValue); }
	AX_INLINE	T	operator++	() 			{ return setByFunc([ ](T v) {return v + 1;  }); }
	AX_INLINE	T	operator--	()			{ return setByFunc([ ](T v) {return v - 1;  }); }
	AX_INLINE	T	operator+=	(T d) 		{ return setByFunc([=](T v) {return v + d; }); }
	AX_INLINE	T	operator-=	(T d)		{ return setByFunc([=](T v) {return v - d; }); }

	T	waitEquals			(T target)			{ return wait([&](const T& v) { return v == target; }); }
	T	waitGreaterThan		(T target)			{ return wait([&](const T& v) { return v >  target; }); }
	T	waitLessThan		(T target)			{ return wait([&](const T& v) { return v <  target; }); }
	T	waitGreaterOrEquals	(T target)			{ return wait([&](const T& v) { return v >= target; }); }
	T	waitLessOrEquals	(T target)			{ return wait([&](const T& v) { return v <= target; }); }

	ScopedLock scopedLock() { return _mdata.scopedLock(); }

	template<class FUNC>
	T setByFunc(FUNC func) { 	// T func(T v)
		T ret;
		{
			auto m = scopedLock();
			ret = m->value = func(m->value);
		}
		return ret;
	}
		
	template<class FUNC>
	T wait(FUNC func) {
		auto m = scopedLock();
		while (!func(m->value)) {
			m.wait();
		}
		return m->value;
	}

private:
	ProtectedData _mdata;
};

template<bool bBroadcast>
using axCondInt = axCondIntT<axInt, bBroadcast>;
