#pragma once

#if AX_USE_METAL

class axMTLStorageBuffer : public axRenderStorageBuffer {
	AX_RTTI_CLASS(axMTLStorageBuffer, axRenderStorageBuffer);
public:
	using Util = axMTLUtil;

	axMTLStorageBuffer(CreateDesc& desc) : Base(desc) {}
	axMTLGpuBuffer*	gpuBuffer() { return ax_type_cast_debug<axMTLGpuBuffer>(_gpuBuffer.ptr()); }

};

#endif
