#pragma once

#include "axSPtr.h"
#include "../thread/axSpinLock.h"
#include "../rtti/axType.h"

class axRefCounted;

class axWeakBlock : public axRefCounted {
public:
	class Data {
	public:
		axRefCounted* obj = nullptr;
	};
	axSpinLockProtected<Data>	data;
};

class axWeakRefBase : public axRefCounted { 
public:
	~axWeakRefBase() {
		if (_weakBlock) {
			auto data = _weakBlock->data.scopedLock();
			data->obj = nullptr;
		}
	}
	axSPtr<axWeakBlock>	_weakBlock;
};

template<class T, axDeleteFunc<T> del = ax_delete<T>>
class axWeak {
public:
	axWeak() = default;
	axWeak(axNull) {}
	axWeak(const axWeak&  r) { _block = r._block; }
	axWeak(      axWeak&& r) { _block = ax_move(r._block); }

	template<class R>
	axWeak(axWeak<R, del> &r)	{ ref(r.ptr()); }

	~axWeak() { unref(); }

	void ref	(T* p) {
		if (!p) {
			return _block.unref();
		}
		
		auto* b = p->_weakBlock.ptr();
		if (b) {
			_block.ref(b);
			return;
		}

		_block.ref(ax_new axWeakBlock());
		p->_weakBlock.ref(_block.ptr());
		auto data = _block->data.scopedLock();
		data->obj = p;
//		AX_ASSERT((void*)data->obj == (void*)p); //T has multiple inheritance ?
	}

	void ref	(const axSPtr<T>& r) {
		ref(ax_const_cast(r.ptr()));
	}

	void unref	() {
		_block.unref();
	}

	void operator=(const axWeak & r)	{ ref(r._p); }
	void operator=(axWeak && r)			{ move(r); }
	void operator=(axNull)				{ unref(); }
	void operator=(const axSPtr<T>& r)	{ ref(ax_const_cast(r.ptr())); }

	void move(axWeak& r)				{ _block.move(r._block); }

	static axWeak<T, del> s_make(T* p) { return axWeak(p); }

	axWeak(T* p)				{ ref(p); }

	axSPtr<T> getSPtr() {
		if (!_block) return nullptr;
		auto data = _block->data.scopedLock();
		return axSPtr_make(static_cast<T*>(data->obj));
	}

	axSPtr<const T> getSPtr() const {
		if (!_block) return nullptr;
		auto data = ax_const_cast(_block->data).scopedLock();
		return axSPtr_make(static_cast<const T*>(data->obj));
	}

private:
	axSPtr<axWeakBlock>	_block;
};