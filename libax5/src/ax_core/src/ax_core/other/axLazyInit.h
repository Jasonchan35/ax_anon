#pragma once
#include "../pointer/axUPtr.h"

// use stack memory
template<class T>
class axLazyInit {
public:
	T* get() {
		if (!_data) {
			ax_call_constructor(_data);
		}
		return _data;
	}

			T* operator->()			{ return get(); }
	const	T* operator->() const	{ return ax_const_cast(this)->get(); }

	explicit operator bool() const { return _data != nullptr; }

	void clear() {
		if (_data) {
			ax_call_destructor(_data);
			_data = nullptr;
		}
	}

private:
	T*	 _data = nullptr;
	char _buf[sizeof(T)];
};


//! use heap memory
template<class T>
class axLazyUPtr {
public:
	T* get() {
		if (!_data) {
			_data.ref(ax_new T());
		}
		return _data.ptr();
	}

	AX_INLINE 			T& operator* ()			{ return *get(); }
	AX_INLINE const	T& operator* () const	{ return *get(); }

	AX_INLINE			T* operator->()			{ return get(); }
	AX_INLINE const	T* operator->() const	{ return ax_const_cast(this)->get(); }

	AX_INLINE explicit operator bool() const { return _data.ptr() != nullptr; }
	AX_INLINE void clear() { _data.unref(); }

private:
	axUPtr<T>	_data;
};
