#pragma once

#include "axPtr.h"
#include "../allocator/axAllocator.h"

class axRefCounted : public axNonCopyable {
public:
	axInt addRef	() const { return ++_refCount; }
	axInt releaseRef() const { return --_refCount; }
	axInt refCount	() const { return _refCount; }

	virtual ~axRefCounted() {
		AX_ASSERT(_refCount == 0);
	}

	virtual void onRefCountZero() {
		ax_delete(this);
	}

private:
	mutable axAtomicInt	_refCount;
};

template<class T>
class axSPtr : public axPtrBase<T> {
	typedef axPtrBase<T>	Base;
public:
	static const axType* s_rtti();

	AX_INLINE	axSPtr() = default;
	AX_INLINE	axSPtr(axNull) noexcept {}
	AX_INLINE	axSPtr(const axSPtr&  p) noexcept { ref(p._p); }
	AX_INLINE	axSPtr(      axSPtr&& r) noexcept { move(r); }
	AX_INLINE	axSPtr(T* p) noexcept { ref(p); }

	//template<class R>
	//AX_INLINE	axSPtr(  axSPtr<R> && r) { move(r); }

	template<class R>
	AX_INLINE	axSPtr(axSPtr<R> &r) { ref(r.ptr()); }

	AX_INLINE	~axSPtr() { unref(); }

	T*	 ref	(T* p);
	void unref	();

	void setToDefaultValue() { unref(); }

	operator T* () & { return _p; }
	operator T* () && = delete;

	AX_INLINE	void operator=(const axSPtr & r)	{ ref(r._p); }
	AX_INLINE	void operator=(axSPtr && r)			{ move(r); }
	AX_INLINE	void operator=(axNull)				{ unref(); }

	template<class R>
	AX_INLINE	void operator=(axSPtr<R> &r)		{ ref(r.ptr()); }

	template<class R>
	AX_INLINE	void operator=(axSPtr<R> && r)		{ move(r); }

	template<class R>
	AX_INLINE	void move(axSPtr<R>& r)	{ 
		if ((void*)&r == this) return;
		ref(r.ptr());
		r.unref();
	}

	bool operator==(const axSPtr& r) const { return _p == r._p; }
	bool operator==(const T*      r) const { return _p == r; }

	bool operator!=(const axSPtr& r) const { return _p != r._p; }
	bool operator!=(const T*      r) const { return _p != r; }

	static axSPtr<T> s_make(T* p) noexcept { return axSPtr(p); }

private:
	////! not allowed in public, please use axSPtr_new /axSPtr_make instead for better exception safety
	//axSPtr(T*  p) noexcept { ref(p); }

	using Base::_p;
};

template<class T> AX_INLINE bool operator==(const T* a,    const axSPtr<T>& b) { return b == a; }
template<class T> AX_INLINE bool operator==(const axNull&, const axSPtr<T>& b) { return b == nullptr; }

template<class T> inline
axSPtr<T> axSPtr_make(T* p) noexcept {
	return axSPtr<T>::s_make(p);
}

#define axSPtr_new(OBJ_CTOR)				axSPtr_make(new(AX_LOC, ax_scope_allocator()) OBJ_CTOR)
#define axSPtr_new_(ALLOCATOR, OBJ_CTOR)	axSPtr_make(new(AX_LOC, ALLOCATOR) OBJ_CTOR)

//--------------------------------

template<class T> AX_INLINE
void axSPtr<T>::unref() {
	if (_p) {
		if (_p->releaseRef() == 0) {
			ax_const_cast(_p)->onRefCountZero();
		}
		_p = nullptr;
	}
}

template<class T> AX_INLINE
T* axSPtr<T>::ref(T * p) {
	if (_p != p) {
		unref();
		_p = p;
		if (p) _p->addRef();
	}
	return p;
}
