#if AX_USE_METAL

#include "axMTLGpuBuffer.h"

void axMTLGpuBuffer::onCreate(const CreateDesc& desc, axInt bufferSize) {
	auto* mtlDevice = Util::mtlDevice();
	MTLResourceOptions opt = 0; //MTLResourceStorageModePrivate; // GPU only
	NSUInteger len = Util::castNSUInt(bufferSize);
	_mtlBuf = [mtlDevice newBufferWithLength:len options:opt];
}

void axMTLGpuBuffer::onUploadToGpu(axInt offset, axByteSpan data) {
	void* p = [_mtlBuf contents];
	auto* dst = reinterpret_cast<axByte*>(p);
	ax_memcpy(dst + offset, data.data(), data.sizeInBytes());
}

#endif
