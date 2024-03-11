#pragma once

#include "../thread/axCondMutex.h"

template<class DATA>
class axResourcePool : public axNonCopyable {
public:
	using Data = DATA;

	axInt size() {
		auto md = _mdata.scopedLock();
		return md->nodes.size();
	}

	void reserve(axInt n) {
		auto md = _mdata.scopedLock();
		md->nodes.reserve(n);
	}

	axUPtr<Data>	pop(bool wait) {
		auto md = _mdata.scopedLock();
		for(;;) {
			if (md->nodes.size()) {
				auto p = ax_move(md->nodes.back());
				md->nodes.decSize(1);
				return p;
			}
			md.wait();
		}
	}

	void add(axUPtr<Data> && p) {
		if (!p) {
			throw axError_Undefined(AX_LOC);
		}

		auto md = _mdata.scopedLock();
		md->nodes.append(ax_move(p));
	}

	class ScopedData : public axNonCopyable {
	public:
		ScopedData(ScopedData && r) {
			_pool = r._pool;
			_data = ax_move(r._data);
			r._data.unref();
		}

		ScopedData(axResourcePool* pool, axUPtr<Data> && data) 
			: _pool(pool)
			, _data(ax_move(data))
		{
		}

		~ScopedData() {
			if (_data) {
				_pool->add(ax_move(_data));
			}
		}

		Data* operator->() { return _data.ptr(); }

	private:
		axResourcePool* _pool = nullptr;
		axUPtr<Data>	_data;
	};

	ScopedData scopedGet() {
		return ScopedData(this, pop(true));
	}

private:
	struct MData {
		axArray<axUPtr<Data>>	nodes;
	};

	axCondMutexProtected<MData, false>	_mdata;
};