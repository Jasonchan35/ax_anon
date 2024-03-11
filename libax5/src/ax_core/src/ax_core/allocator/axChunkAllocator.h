#pragma once

#include "axAllocator.h"

//!TODO
class axChunkAllocator : public axAllocator {
	typedef axAllocator	Base;
public:
	axChunkAllocator(axAllocator* parent) 
		: Base(parent) 
	{
		setName(AX_SZ("axChunkAllocator"));
	}

private:
	virtual Result	onAlloc		(axInt actualSize, const Request& req) {
		//TODO
		return _parent->onAlloc(actualSize, req); 
	}
};