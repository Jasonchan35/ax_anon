#pragma once

#include "axAllocator.h"

class axLinearAllocator : public axAllocator {
	using Base = axAllocator;
public:
	axLinearAllocator(axAllocator* parent, axInt nextChunkSize)
		: Base(parent)
		, _nextChunkSize(nextChunkSize)
	{
		setName(AX_SZ("axLinearAllocator"));
	}

	void reset() {
		auto lock = _lock.scopedLock();

		while (_chunkList.size() > 1) {
			_chunkList.popTail();
		}

		if (auto* c = _chunkList.tail()) {
			c->reset();
		}
	}

	void onFormat(axFormat& f) const;

private:
	class Chunk : public ChunkBase, public axDListNode<Chunk> {
		using Base = ChunkBase;
	public:
		Chunk(axAllocator* allocator) : Base(allocator) {}
		~Chunk() { reset(); }

		virtual void onDealloc(axInt actualSize, BlockHeader* hdr) {
			_freedSize += actualSize;
		}


		void reset() {
			AX_ASSERT(_usedSize == _freedSize);
			_usedSize = 0;
			_freedSize = 0;
		}

		axInt availableSize() const { return _bufferSize - _usedSize; }

		char* _buffer = nullptr;
		axInt _bufferSize = 0;
		axInt _usedSize = 0;
		axInt _freedSize = 0;
	};

	axInt _nextChunkSize = 0;
	axDList<Chunk>	_chunkList;

	virtual Result onAlloc	(axInt actualSize, const Request& req) override;
};

inline
axAllocator::Result axLinearAllocator::onAlloc(axInt actualSize, const Request& req)
{
	Result o(nullptr, nullptr);
	if (actualSize <= 0) {
		return o;
	}

	Chunk* chunk = nullptr;
	for (auto& c : _chunkList) {
		if (c.availableSize() >= actualSize) {
			chunk = &c;
		}
	}

	if (!chunk) {
		if (_nextChunkSize < actualSize) {
			_nextChunkSize = actualSize;
		}

		_nextChunkSize = ax_next_pow2(_nextChunkSize);

		Request chunkReq(AX_LOC, _parent, ax_sizeof(Chunk) + _nextChunkSize, kDefaultAligenment);
		auto* p = _parent->alloc(chunkReq);
		chunk = new(p) Chunk(this);

		_chunkList.append(chunk);

		chunk->_buffer = reinterpret_cast<char*>(chunk + 1);
		chunk->_bufferSize = _nextChunkSize;
		_nextChunkSize *= 2;
	}

	o.chunk = chunk;
	o.block = chunk->_buffer + chunk->_usedSize;
	chunk->_usedSize += actualSize;
	return o;
}