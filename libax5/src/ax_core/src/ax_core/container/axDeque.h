#pragma once

#include "axArray.h"
#include "axDList.h"

template<class T, axInt CHUNK_SIZE = 32>
class axDeque {
public:
	static const axInt kChunkSize = CHUNK_SIZE;

	void append(const T& v) { _append(v); }

	T popHead() { return _popHead(); }

private:

	void _append(const T& v) {
		Chunk* c = nullptr;
		if (_list.size()) {
			auto* tail = _list.tail();
			if (tail->data.size() < CHUNK_SIZE)
				c = tail;
		}

		if (!c) {
			c = _freeList.popHead();
			if (!c) c = ax_new Chunk();
			_list.append(c);
		}

		c->append(v);
	}

	T& front() {
		auto* c = _list.head();
		AX_ASSERT(c);
		return c->front();
	}

	T _popHead() {
		auto* c = _list.head();
		auto ret = c->popHead();
		if (c->size() <= 0) {
			_list.remove(c);
			_freeList.insert(c);
		}
		return ret;
	}


	class Chunk : public axLocalBuffer<T, CHUNK_SIZE> {
		using LocalBuf = axLocalBuffer<T, CHUNK_SIZE>;
	public:
		axInt head = 0;
		axInt tail = 0;

		axInt size() { return tail - head; }

		T& at(axInt i) {
			_debug_checkBound(i);
			return data()[i];
		}

		T* data() { return LocalBuf::localBufPtr(); }

		void append(const T& v) {
			AX_ASSERT(tail < CHUNK_SIZE);
			ax_call_constructor(data() + tail, v);
			tail++;
		}

		void _debug_checkBound(axInt i) {
			if (i < head || i >= tail)
				throw axError_Undefined(AX_LOC);
		}
	};

	axDList<Chunk> _list;
	axDList<Chunk> _freeList;
};


