#pragma once

#include "axAllocator.h"

class axSystemAllocator : public axAllocator {
	typedef axAllocator	Base;
public:
	axSystemAllocator(axAllocator* parent)
		: Base(parent)
		, _chunk(this)
	{
		setName(AX_SZ("axSystemAllocator"));
	}

private:
	class Chunk : public ChunkBase {
		using Base = ChunkBase;
	public:
		Chunk(axAllocator* allocator) : Base(allocator) {}

		virtual void onDealloc(axInt actualSize, BlockHeader* hdr);
	};

	Chunk _chunk;

	virtual Result onAlloc	(axInt numBytes, const Request& req) override;
};

