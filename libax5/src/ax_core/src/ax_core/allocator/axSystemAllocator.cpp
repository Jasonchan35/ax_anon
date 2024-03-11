#include <ax_core/allocator/axSystemAllocator.h>

axAllocator::Result axSystemAllocator::onAlloc(axInt actualSize, const Request& req)
{
	Result o(nullptr, nullptr);
	if (actualSize <= 0) {
		return o;
	}

	o.chunk = &_chunk;
	o.block = ax_system_malloc(actualSize, req.alignment);
	return o;
}

void axSystemAllocator::Chunk::onDealloc(axInt actualSize, BlockHeader* hdr) {
	ax_system_free(hdr);
}
