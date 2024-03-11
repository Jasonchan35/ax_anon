#pragma once

#include <ax_core/base/ax_base.h>

template<class T>
class axPtrBase {
public:
	axPtrBase(T* p = nullptr) : _p(p) {}
	~axPtrBase() { _p = nullptr; }

	AX_INLINE 			T* ptr() && = delete;

	AX_INLINE 			T* ptr() &				{ return  _p; }
	AX_INLINE const	T* ptr() const &		{ return  _p; }

	AX_INLINE 			T* operator->()			{ return  _p; }
	AX_INLINE const	T* operator->() const	{ return  _p; }

	AX_INLINE 			T& operator* ()			{ return *_p; }
	AX_INLINE const	T& operator* () const	{ return *_p; }

	AX_INLINE explicit operator bool() const	{ return _p != nullptr; }

	void onFormat(axFormat& f) const;

protected:
	T* _p;
};

template<class T>
class axPtr : public axPtrBase<T> {
	typedef axPtrBase<T> Base;
	using Base::_p;
public:
	axPtr(T* p = nullptr) : Base(p) {}

	AX_INLINE void set			(T* p)		{ _p = p; }
	AX_INLINE void operator=	(T* p)		{ set(p); }
	AX_INLINE void unref		()			{ _p = nullptr; }
	
	AX_INLINE 		 T*& ptrRef	() &		{ return  _p; }
	AX_INLINE const T*& ptrRef	() const &	{ return  _p; }

	AX_INLINE operator       T*() &		{ return  _p; }
	AX_INLINE operator const T*() const &	{ return  _p; }
	
	AX_INLINE void doDelete() { ax_delete(_p); _p = nullptr; }
};

template<class T> inline axPtr<T> axPtr_make(T* p) { return axPtr<T>(p); }
template<class T> inline void ax_delete(axPtr<T>& p) { ax_delete(p.ptr()); p.unref(); }

template<class T>
class axNotNull : public axPtrBase<T> {
	typedef axPtrBase<T>	Base;
	using Base::_p;
public:
	AX_INLINE	axNotNull(axNotNull && r) 			{ move(r); }
					~axNotNull()						{ _checkNull(); }

					void setPtr		(T* p)				{ _p = p; _checkNull(); }	

					void move		(axNotNull && r)	{ _p = r._p; r._p = nullptr; }
					void operator=	(axNotNull && r)	{ move(r); }

					void reset		()					{ _p = nullptr; }

	AX_INLINE	operator		T*	()				{ return _p; }
	AX_INLINE	operator const	T*	() const		{ return _p; }

	static	axNotNull s_make(T* p) { return axNotNull(p); }
private:
	void _checkNull() {
		if (Base::_p == nullptr) throw axError_NullPointer();
	}

	AX_INLINE	explicit axNotNull(T* p) : Base(p)			{ _checkNull(); }
};

template<class T> inline axNotNull<T> axNotNull_make(T* p) { return axNotNull<T>::s_make(p); }

