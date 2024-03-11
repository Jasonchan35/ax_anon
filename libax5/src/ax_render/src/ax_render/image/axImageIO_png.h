#pragma once

#if !AX_BUILD_ax_render
	#error "only include this file when building ax_render"
#endif

#include <png.h>
#include <ax_render/image/axImage.h>

namespace axImageIO_png {

class Error : public axError {};
class Error_StartDecompress			: public Error {};
class Error_ReadHeader				: public Error {};
class Error_ReadPixels				: public Error {};
class Error_ReadUnsupportedFormat	: public Error {};

class Reader {
public:
	void load(axImage& img, axByteSpan data, axColorType expectType = axColorType::Unknown);

	~Reader();

	static void s_onRead(png_structp png, png_bytep dest, png_size_t len);
	void onRead(png_bytep dest, png_size_t len);

	void setReadFilter(axColorType out_type, int in_type, int in_bit, bool in_palette_has_alpha);

	bool error_longjmp_restore_point();

	png_structp	_png{nullptr};
	png_infop	_info{nullptr};

	axByteSpan _data;
	axByteSpan _remain;
};

} //namespace
