#if AX_USE_DX11

#include <ax_render/axRenderContext.h>
#include "axDX11Renderer.h"
#include "axDX11GpuBuffer.h"

void axDX11GpuBuffer::onCreate(const CreateDesc& desc, axInt bufferSize) {
	if (bufferSize <= 0) throw axError_Undefined(AX_LOC);
	if (desc.byteStride <= 0) throw axError_Undefined(AX_LOC);

	D3D11_BUFFER_DESC bd = {};

	bd.ByteWidth = Util::castUINT(ax_multiple_of(bufferSize, axInt(16)));
	bd.StructureByteStride = Util::castUINT(desc.byteStride);

	switch (desc.type) {
		case Type::Vertex: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Index: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Const: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Stage: {
			bd.Usage			= D3D11_USAGE_DEFAULT;
			bd.BindFlags		= D3D11_BIND_UNORDERED_ACCESS;
			bd.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Storage: {
			bd.Usage			= D3D11_USAGE_DEFAULT;
			bd.BindFlags		= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
//			bd.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bd.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}break;
		default: throw axError_Undefined(AX_LOC);
	}

	auto hr = Util::d3dDevice()->CreateBuffer(&bd, nullptr, _d3dBuf.ptrForInit());
	Util::throwIfError(hr);
}

void axDX11GpuBuffer::onUploadToGpu(axInt offset, axByteSpan data) {
	D3D11_MAPPED_SUBRESOURCE mapped = {};

	auto* ctx = Util::d3dContext();
	auto hr = ctx->Map(_d3dBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	Util::throwIfError(hr);

	axByte* dst = reinterpret_cast<axByte*>(mapped.pData);
	ax_memcpy(dst + offset, data.data(), data.sizeInBytes());

	ctx->Unmap(_d3dBuf, 0);
}

#endif //AX_USE_DX11