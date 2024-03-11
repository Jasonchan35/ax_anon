#include "axImage.h"
#include "axImageIO_png.h"
#include "axImageIO_jpeg.h"
#include "axImageIO_dds.h"

axImage::axImage(axImage && r) {
	operator=(ax_move(r));
}

void axImage::operator=(axImage && r) {
	_info = r._info;
	_pixelData.move(r._pixelData);
	r.clear();
}

void axImage::clear() {
	_info = axImageInfo();
	_pixelData.clear();
}

void axImage::loadFile(axStrView filename) {
	auto ext = axPath::extension(filename);
	if (ext.equals("png", true)) {
		return loadPngFile(filename);
	}

	if (ext.equals("jpg", true) || ext.equals("jpeg", true)) {
		return loadJpegFile(filename);
	}

	if (ext.equals("dds", true)) {
		return loadDdsFile(filename);
	}

	throw axError_Undefined(AX_LOC);
}

void axImage::loadPngFile (axStrView filename) { axMemMap mm(filename); loadPngMem (mm); }
void axImage::loadJpegFile(axStrView filename) { axMemMap mm(filename); loadJpegMem(mm); }
void axImage::loadDdsFile (axStrView filename) { axMemMap mm(filename); loadDdsMem (mm); }

void axImage::loadPngMem(axByteSpan data) {
	axImageIO_png::Reader r;
	r.load(*this, data);
}

void axImage::loadJpegMem(axByteSpan data) {
	axImageIO_jpeg::Reader r;
	r.load(*this, data);
}


void axImage::loadDdsMem(axByteSpan data) {
	axImageIO_dds::Reader r;
	r.load(*this, data);
}

void axImage::create(axColorType colorType, axInt width, axInt height) {
	create(colorType, width, height, width * axColorUtil::pixelSizeInBytes(colorType));
}

void axImage::create(axColorType colorType, axInt width, axInt height, axInt strideInBytes) {
	create(colorType, width, height, strideInBytes, 1, strideInBytes * height);
}

void axImage::create(axColorType colorType, axInt width, axInt height, axInt strideInBytes, axInt mipmapCount, axInt pixelDataSize) {
	_info.colorType = colorType;
	_info.size.set(width, height);
	_info.strideInBytes = strideInBytes;
	_info.mipmapCount = mipmapCount;

	try{
		_pixelData.resizeNoCopy(pixelDataSize);
	}catch(...) {
		clear();
		throw;
	}
}

void axImage::subImage(axVec2i pos, const axImageInfo& srcInfo, axByteSpan srcPixelData) {
	if (srcInfo.colorType != _info.colorType) {
		throw axError_Undefined(AX_LOC);
	}

	auto pixelSizeInBytes = _info.pixelSizeInBytes();

	if (srcInfo.pixelSizeInBytes() != pixelSizeInBytes) {
		throw axError_Undefined(AX_LOC);
	}

	auto start = pos;
	auto end   = pos + srcInfo.size;

	auto zero  = axVec2i(0,0);

	ax_clamp_it(start, zero, size());
	ax_clamp_it(end,   zero, size());

	if (end.x <= start.x || end.y <= start.y) return;

		  axByte* dst = &rowBuffer (start.y)[start.x];
	const axByte* src = srcPixelData.data();
	const axByte* srcEnd = srcPixelData.end();

	axInt dstStride = strideInBytes();
	axInt srcStride = srcInfo.strideInBytes;

	if (pos.x == 0 && _info.size.x == srcInfo.size.x && _info.strideInBytes == srcInfo.strideInBytes) {
		//direct copy all rows
		auto n = pixelSizeInBytes * _info.size.x * srcInfo.size.y;
		if (src + n > srcEnd) {
			throw axError_Undefined(AX_LOC);
		}
		ax_memcpy(dst, src, n);
		return;
	}

	auto rows = end.y - start.y;
	axInt rowSizeInBytes = pixelSizeInBytes * (end.x - start.x);

	for (auto y = 0; y < rows; y++) {
		if (src + rowSizeInBytes > srcEnd) {
			throw axError_Undefined(AX_LOC);
		}
		ax_memcpy(dst, src, rowSizeInBytes);
		dst += dstStride;
		src += srcStride;
	}
}

void axImage::copy(const axImage& srcImage) {
	clear();
	create(srcImage.colorType(), srcImage.width(), srcImage.height(), srcImage.strideInBytes());
	subImage(axVec2i(0,0), srcImage.info(), srcImage.pixelData());
}

template<class T>
static void axImage_convertToPremultipiedAlpha(axImage& img) {
	auto h = img.height();

	for (axInt y = 0; y < h; y++) {
		auto r = img.row_noCheck<T>(y);
		for (auto& p : r) {
			p = p.toPremultipliedAlpha();
		}
	}
}

void axImage::convertToPremultipiedAlpha() {
	auto alphaBits = axColorUtil::alphaBits(_info.colorType);
	if (alphaBits <= 0) return;

	if (axColorUtil::isCompressedType(_info.colorType))
		return;

	switch (_info.colorType) {
	#define AX_MACRO_OP(E, VALUE, SUFFIX) \
		case axColorType::RGBA##SUFFIX:	axImage_convertToPremultipiedAlpha<axColor##RGBA##SUFFIX>(*this); break; \
		case axColorType::BGRA##SUFFIX:	axImage_convertToPremultipiedAlpha<axColor##BGRA##SUFFIX>(*this); break; \
		case axColorType::LA  ##SUFFIX:	axImage_convertToPremultipiedAlpha<axColor##LA  ##SUFFIX>(*this); break; \
	//-------
		axColorElementType_EnumList(AX_MACRO_OP);
	#undef AX_MACRO_OP
		default: AX_ASSERT_NOT_IMPLEMENTED(); break;
	}
}
