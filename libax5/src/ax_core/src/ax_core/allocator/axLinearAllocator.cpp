#include "axLinearAllocator.h"

void axLinearAllocator::onFormat(axFormat& f) const {
	for (auto& c : _chunkList) {
		f.format("Chunk {?}\n", reinterpret_cast<const void*>(&c));
		f.format("  bufferSize = {?}\n", c._bufferSize);
		f.format("  usedSize   = {?}\n", c._usedSize);
		f.format("  freedSize  = {?}\n", c._freedSize);
	}
}