#if AX_USE_DX11

#include <ax_render/axRenderContext.h>
#include "axDX11Texture.h"
#include "axDX11Renderer.h"

axDX11Texture2D::axDX11Texture2D(CreateDesc& desc) 
	: Base(desc)
{
	auto* dev = Util::d3dDevice();

	D3D11_TEXTURE2D_DESC texDesc = {};
	ax_safe_assign(texDesc.Width,     desc.size.x);
	ax_safe_assign(texDesc.Height,    desc.size.y);
	ax_safe_assign(texDesc.MipLevels, desc.mipmapCount);
	texDesc.ArraySize = 1;
	texDesc.Format = Util::getDxColorType(desc.colorType);
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET
	texDesc.CPUAccessFlags = 0; // D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
	texDesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS

	D3D11_SUBRESOURCE_DATA initData = {};


	if (desc.uploadRequest) {
		auto& info = desc.uploadRequest->imageInfo;
		initData.pSysMem = desc.uploadRequest->pixelData.data();
		ax_safe_assign(initData.SysMemPitch, info.strideInBytes);
		initData.SysMemSlicePitch = 0;
	}

	auto hr = dev->CreateTexture2D(&texDesc, desc.uploadRequest ? &initData : nullptr, _tex.ptrForInit());
	Util::throwIfError(hr);

//---------
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter   = Util::getDxTextureFilter(desc.samplerState.filter);
	samplerDesc.AddressU = Util::getDxTextureWrap(desc.samplerState.wrapU);
	samplerDesc.AddressV = Util::getDxTextureWrap(desc.samplerState.wrapV);
	samplerDesc.AddressW = Util::getDxTextureWrap(desc.samplerState.wrapW);
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = desc.samplerState.minLOD;
	samplerDesc.MaxLOD = desc.samplerState.maxLOD;

	hr = dev->CreateSamplerState(&samplerDesc, _samplerState.ptrForInit());
	Util::throwIfError(hr);

//---------
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc = {};
	rvDesc.Format = texDesc.Format;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rvDesc.Texture2D.MipLevels = texDesc.MipLevels ? texDesc.MipLevels : -1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	hr = dev->CreateShaderResourceView(_tex, &rvDesc, _resourceView.ptrForInit());
	Util::throwIfError(hr);
}

void axDX11Texture2D::onUploadRequest(axDX11CommandDispatcher& dispatcher, UploadRequest& upload) {
	AX_ASSERT(upload.tex.ptr() == this);
	if (!_tex) {
		throw axError_Undefined(AX_LOC);
	}

	auto* ctx = Util::d3dContext();

	auto& info = upload.imageInfo;

	axRect2i rect(upload.offset, info.size);

	D3D11_BOX box;
	box.left   = (UINT)rect.xMin();
	box.right  = (UINT)rect.xMax();
	box.top    = (UINT)rect.yMin();
	box.bottom = (UINT)rect.yMax();
	box.front  = 0;
	box.back   = 1;

	ctx->UpdateSubresource(	_tex.ptr(), 0, &box, 
							upload.pixelData.data(),
							Util::castUINT(info.strideInBytes),
							0);
}

#endif //AX_USE_DX11