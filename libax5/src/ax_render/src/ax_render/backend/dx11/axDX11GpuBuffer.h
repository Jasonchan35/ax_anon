#pragma once

#if AX_USE_DX11

#include "../../buffer/axRenderGpuBuffer.h"
#include "axDX11CommonBase.h"

class axDX11Renderer;
class axDX11GpuBuffer : public axRenderGpuBuffer {
	AX_RTTI_CLASS(axDX11GpuBuffer, axRenderGpuBuffer);
public:
	using Util = axDX11Util;

	virtual void onCreate(const CreateDesc& desc, axInt bufferSize) final;
	virtual void onUploadToGpu(axInt offset, axByteSpan data) final;

	axComPtr<ID3D11Buffer> _d3dBuf;
};

#endif //AX_USE_DX11
