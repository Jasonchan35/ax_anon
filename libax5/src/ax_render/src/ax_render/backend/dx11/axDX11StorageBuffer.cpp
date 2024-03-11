#if AX_USE_DX11

#include "axDX11StorageBuffer.h"

axDX11StorageBuffer::axDX11StorageBuffer(CreateDesc& desc) 
	: Base(desc)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC vd = {};
	vd.Format = DXGI_FORMAT_R32_TYPELESS;
	vd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	vd.Buffer.FirstElement = 0;
	vd.Buffer.NumElements = Util::castUINT(desc.bufferSize / 4); // Util::castUINT(desc.bufferSize / desc.byteStride); 
	vd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

	auto* dev = Util::d3dDevice();
	auto hr = dev->CreateUnorderedAccessView(gpuBuffer()->_d3dBuf, &vd, _uav.ptrForInit());
	Util::throwIfError(hr);
}

#endif
