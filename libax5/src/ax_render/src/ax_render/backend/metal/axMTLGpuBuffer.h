#pragma once

#if AX_USE_METAL

#include "../../buffer/axRenderMultiGpuBuffer.h"
#include "axMTLCommonBase.h"

class axMTLGpuBuffer : public axRenderGpuBuffer {
	AX_RTTI_CLASS(axMTLGpuBuffer, axRenderGpuBuffer)
public:
	using Util = axMTLUtil;
	id<MTLBuffer>	mtlBuffer() { return _mtlBuf; }

	virtual void onCreate(const CreateDesc& desc, axInt bufferSize) final;
	virtual void onUploadToGpu(axInt offset, axByteSpan data) final;

private:
	id<MTLBuffer> _mtlBuf;
};

#endif
