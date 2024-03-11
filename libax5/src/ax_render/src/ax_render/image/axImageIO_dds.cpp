#include "axImageIO_dds.h"

namespace axImageIO_dds {

Reader::Reader() {
}

Reader::~Reader() {
}

void Reader::_readHeader(axBinDeserializer& de, DDS_HEADER& hdr) {
	de.io_le(hdr.dwSize);
	de.io_le(hdr.dwHeaderFlags);
	de.io_le(hdr.dwHeight);
	de.io_le(hdr.dwWidth);
	de.io_le(hdr.dwPitchOrLinearSize);
	de.io_le(hdr.dwDepth);
	de.io_le(hdr.dwMipMapCount);
	de.advance(ax_sizeof(hdr.dwReserved1[0]) * 11);

	de.io_le(hdr.ddspf.dwSize);
	de.io_le(hdr.ddspf.dwFlags);
	de.io_le(hdr.ddspf.dwFourCC);
	de.io_le(hdr.ddspf.dwRGBBitCount);
	de.io_le(hdr.ddspf.dwRBitMask);
	de.io_le(hdr.ddspf.dwGBitMask);
	de.io_le(hdr.ddspf.dwBBitMask);
	de.io_le(hdr.ddspf.dwABitMask);

	de.io_le(hdr.dwSurfaceFlags);
	de.io_le(hdr.dwCubemapFlags);
	de.advance(ax_sizeof(hdr.dwReserved2[0]) * 3);

	if (hdr.dwSize != sizeof(hdr))
		throw axError_Undefined(AX_LOC);

	if (hdr.ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		throw axError_Undefined(AX_LOC);

	if (!(hdr.ddspf.dwFlags & DDS_FOURCC))
		throw axError_Undefined(AX_LOC);
}

void Reader::_readHeader10(axBinDeserializer& de, DDS_HEADER_DXT10& hdr10) {
	uint32_t tmp32;

	de.io_le(tmp32);
	hdr10.dxgiFormat = static_cast<DXGI_FORMAT>(tmp32);

	de.io_le(tmp32);
	hdr10.resourceDimension = static_cast<D3D10_RESOURCE_DIMENSION>(tmp32);

	de.io_le(hdr10.miscFlag);
	de.io_le(hdr10.arraySize);
	de.io_le(hdr10.miscFlags2);
}

void Reader::load(axImage& img, axByteSpan data, axColorType expectType) {
	axBinDeserializer de(data);

	uint32_t sign;
	de.io_le(sign);

	if (sign != axFourCC("DDS "))
		throw axError_Undefined(AX_LOC);

	DDS_HEADER hdr;
	_readHeader(de, hdr);

	auto colorType = axColorType::Unknown;

	if (hdr.ddspf.dwFourCC == axFourCC("DX10")) {
		DDS_HEADER_DXT10 hdr10;
		_readHeader10(de, hdr10);

		if (hdr10.resourceDimension != D3D10_RESOURCE_DIMENSION_TEXTURE2D)
			throw axError_Undefined(AX_LOC);

		switch (hdr10.dxgiFormat)
		{
			case DXGI_FORMAT_BC1_UNORM:  colorType = axColorType::BC1; break;
			case DXGI_FORMAT_BC2_UNORM:  colorType = axColorType::BC2; break;
			case DXGI_FORMAT_BC3_UNORM:  colorType = axColorType::BC3; break;
			case DXGI_FORMAT_BC4_UNORM:  colorType = axColorType::BC4; break;
			case DXGI_FORMAT_BC5_UNORM:  colorType = axColorType::BC5; break;
			case DXGI_FORMAT_BC7_UNORM:  colorType = axColorType::BC7; break;
			default: throw axError_Undefined(AX_LOC);
		}
	}else{
		switch (hdr.ddspf.dwFourCC) {
			case axFourCC("DXT1"): colorType = axColorType::BC1; break;
			case axFourCC("DXT2"): colorType = axColorType::BC2; break;
			case axFourCC("DXT3"): colorType = axColorType::BC2; break;
			case axFourCC("DXT4"): colorType = axColorType::BC3; break;
			case axFourCC("DXT5"): colorType = axColorType::BC3; break;

			case axFourCC("BC4U"): colorType = axColorType::BC4; break;
			case axFourCC("BC5U"): colorType = axColorType::BC5; break;
			case axFourCC("DXT7"): colorType = axColorType::BC7; break;
			default: throw axError_Undefined(AX_LOC);
		}
	}

	if (hdr.dwWidth % 4 || hdr.dwHeight % 4)
		throw axError_Undefined(AX_LOC);

	axInt mipmapCount = 1;

	if (hdr.dwMipMapCount > 1) {
		uint32_t w = hdr.dwWidth;
		uint32_t h = hdr.dwHeight;
		for (uint32_t i = 0; i < hdr.dwMipMapCount; i++) {
			if (w & 0x3 || h & 0x3)
				break; // bypass non-multiple of 4 level

			w >>= 1;
			h >>= 1;
			mipmapCount = i + 1;
		}
	}

	axInt blockSize = axColorUtil::bytesPerPixelBlock(colorType);
	axInt strideInBytes = ax_max(1U, ((hdr.dwWidth + 3)/4)) * blockSize;

	img.create(colorType, hdr.dwWidth, hdr.dwHeight, strideInBytes, mipmapCount, de.remain().size());
	img.pixelData().copyValues(de.remain());
}

} //namespace