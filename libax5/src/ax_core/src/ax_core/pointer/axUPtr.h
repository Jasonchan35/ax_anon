#pragma once

#include "axPtr.h"
#include "../allocator/axAllocator.h"

//! Unique Pointer
template<class T>
class axUPtr : public axPtrBase<T>, public axNonCopyable {
	typedef axPtrBase<T>	Base;
	using Base::_p;
public:
	AX_INLINE	axUPtr() = default;
	AX_INLINE	axUPtr(axUPtr && r) noexcept { move(r); }
	AX_INLINE	axUPtr(std::nullptr_t) {};

	template<class R>
    AX_INLINE	axUPtr(axUPtr<R> && r) noexcept { move(r); }

	AX_INLINE	~axUPtr() noexcept { unref(); }

	AX_INLINE	T*		ref			(T* p) & noexcept	{ if (_p != p) { unref(); _p = p; } return p; };
	AX_INLINE	void	unref		() noexcept			{ ax_delete_set_null(_p); }
	AX_INLINE	T*		detach		() noexcept			{ auto* p = _p; _p = nullptr; return p; }

	template<class R>
	AX_INLINE	void	move		(axUPtr<R> &  r) noexcept {
		if ((void*)&r == this) return;
		ref(r.detach());
	}

	template<class R>
	AX_INLINE	void	operator=	(axUPtr<R> && r) noexcept { move(r); }

	AX_INLINE	bool operator==(const T* p) const noexcept { return _p == p; }
	AX_INLINE	bool operator!=(const T* p) const noexcept { return _p != p; }

	static  axUPtr  s_make(T* p) noexcept { return axUPtr(p); }

private:
	//! not allowed in public, please use axUPtr_new / axUPtr_make instead for better exception safety
	AX_INLINE	explicit axUPtr(T* p) : Base(p) {}

	// not allowed for ownership safety reason
	// operator T* () { return _p; }
};

template<class T> AX_INLINE bool operator==(const T* a,    const axUPtr<T>& b) { return b == a; }
template<class T> AX_INLINE bool operator==(const axNull&, const axUPtr<T>& b) { return b == nullptr; }

template<class T> inline 
axUPtr<T> axUPtr_make(T* p) noexcept { 
	return axUPtr<T>::s_make(p); 
}

#define axUPtr_new(OBJ_CTOR)				axUPtr_make(new(AX_LOC, ax_scope_allocator()) OBJ_CTOR)
#define axUPtr_new_(ALLOCATOR, OBJ_CTOR)	axUPtr_make(new(AX_LOC, ALLOCATOR) OBJ_CTOR)
