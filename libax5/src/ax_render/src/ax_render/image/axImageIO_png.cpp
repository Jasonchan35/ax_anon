#include "axImageIO_png.h"

namespace axImageIO_png {

Reader::~Reader() {
	if (_info) {
		png_destroy_info_struct(_png, &_info);
		_info = nullptr;
	}
	if (_png) {
		png_destroy_read_struct(&_png, nullptr, nullptr);
		_png = nullptr;
	}
}

bool Reader::error_longjmp_restore_point() {
	// !!! call this function before any libpng C-function that might longjmp()
	// to avoid any C++ destructor or exception try/catch block happen in between

	#if AX_COMPILER_VC
		#pragma warning(push) 
		#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	#endif

	// longjmp() to here from pngReportError()
	return setjmp(png_jmpbuf(_png)) != 0;

	#if AX_COMPILER_VC
		#pragma warning(pop) 
	#endif
}

void Reader::load(axImage& img, axByteSpan data, axColorType expectType) {
	_data = data;
	_remain = data;
	AX_ASSERT(!_png);

	if (data.size() < 8 || png_sig_cmp(data.data(), 0, 8)) {
		throw axError_Undefined(AX_LOC); // not png format
	}

	_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!_png) {
		throw axError_Undefined(AX_LOC);
	}

	_info = png_create_info_struct(_png);
	if (!_info) {
		throw Error_StartDecompress();
	}

	png_set_read_fn( _png, this, &s_onRead );

	//-----
	if (error_longjmp_restore_point()) {
		throw Error_ReadHeader();
	}
	png_read_info( _png, _info );

	png_uint_32 in_width  = 0;
	png_uint_32 in_height = 0;
	int in_bit = 0;
	int in_color_type = 0;
	int in_interlace = 0;
	bool in_palette_has_alpha = false;

	//-----
	if (error_longjmp_restore_point()) {
		throw axError_Undefined(AX_LOC);
	}
	png_get_IHDR( _png, _info, &in_width, &in_height, &in_bit, &in_color_type, &in_interlace, nullptr, nullptr );	

	auto out_color_type = axColorType::Unknown;
	if (expectType == axColorType::Unknown) {
		if (in_bit == 8) {
			switch (in_color_type) {
				case PNG_COLOR_TYPE_RGB:		out_color_type = axColorType::RGBAb;	break; //DirectX doesn't support RGBb, so convert to RGBAb instead
				case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = axColorType::RGBAb;	break;
				case PNG_COLOR_TYPE_GRAY:		out_color_type = axColorType::Lb;		break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:	out_color_type = axColorType::LAb;		break;
				case PNG_COLOR_TYPE_PALETTE:	{
					//get palette transparency table
					png_bytep		trans_alpha = nullptr;
					int				num_trans   = 0;
					png_color_16p	trans_color = nullptr;

					//-----
					if (error_longjmp_restore_point()) {
						throw Error_ReadHeader();
					}
					png_get_tRNS(_png, _info, &trans_alpha, &num_trans, &trans_color);

					out_color_type = axColorType::RGBAb;
					
					if (trans_alpha) {
						in_palette_has_alpha = true;
//						out_color_type = axColorType::RGBAb;
					}else{
//						out_color_type = axColorType::RGBb;
					}
					
				}break;
			}
		} else if (in_bit == 16) {
			switch (in_color_type) {
				case PNG_COLOR_TYPE_RGB:		out_color_type = axColorType::RGBAs;	break; //DirectX doesn't support RGBb, so convert to RGBAb instead
				case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = axColorType::RGBAs;	break;
				case PNG_COLOR_TYPE_GRAY:		out_color_type = axColorType::Ls;		break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:	out_color_type = axColorType::LAs;		break;
			}
		} else {
			throw Error_ReadUnsupportedFormat();
		}
	}

	if (out_color_type == axColorType::Unknown) {
		throw Error_ReadUnsupportedFormat();
	}

	//-----
	setReadFilter(out_color_type, in_color_type, in_bit, in_palette_has_alpha);

	axInt width  = 0;
	axInt height = 0;
	ax_safe_assign(width,  in_width);
	ax_safe_assign(height, in_height);

	if (width <= 0 || height <= 0) {
		throw Error_ReadHeader();
	}

	img.create(out_color_type, width, height);

	axArray<png_bytep, 2048> rows;
	rows.resize(height);
	for (axInt y = 0; y < height; y++) {
		rows[y] = (png_bytep)img.rowBuffer(y).data();
	}

	png_set_rows(_png, _info, rows.data());

	//-----
	if (error_longjmp_restore_point()) {
		throw Error_ReadPixels();
	}
	png_read_image(_png, rows.data());
}

void Reader::s_onRead( png_structp png, png_bytep dest, png_size_t len ) {
	auto* p = reinterpret_cast<Reader*>(png_get_io_ptr( png ));
	if( !p ) {
		throw axError_Undefined(AX_LOC);
	}
	p->onRead(dest, len);
}

void Reader::onRead(png_bytep dest, png_size_t len) {
	axInt n;
	ax_safe_assign(n, len);
	if (n > _remain.size()) {
		throw axError_Undefined(AX_LOC);
	}
	ax_memcpy(dest, _remain.data(), n);
	_remain = _remain.sliceFrom(n);
}

void Reader::setReadFilter(axColorType out_color_type, int in_color_type, int in_bit, bool in_palette_has_alpha) {
	auto* elementType = axColorUtil::elementType(out_color_type);
	if (!elementType) throw Error_ReadUnsupportedFormat();

// 8 <-> 16 bit
	switch (elementType->internalType()) {
		case axType_InternalType::UInt8: {
			switch (in_bit) {
				case 8:		break;
				case 16:	png_set_scale_16(_png); break;
				default:	throw Error_ReadUnsupportedFormat();
			}
		}break;

		case axType_InternalType::UInt16: {
			switch (in_bit) {
				case 8:		png_set_expand_16(_png); break;
				case 16:	break;
				default:	throw Error_ReadUnsupportedFormat();
			}
			#if AX_CPU_ENDIAN_LITTLE
				png_set_swap(_png);
			#endif
		}break;

		default: throw Error_ReadUnsupportedFormat();
	}

// palette
	if (in_color_type & PNG_COLOR_MASK_PALETTE) {
		png_set_palette_to_rgb(_png);
	}

// alpha channel
	if (axColorUtil::alphaBits(out_color_type)) {
		// add alpha channel if missing
		if (in_color_type & PNG_COLOR_MASK_PALETTE) {
			if (!in_palette_has_alpha) {
				png_set_filler(_png, 0xffff, PNG_FILLER_AFTER);
			}
		}
		if (!(in_color_type & PNG_COLOR_MASK_ALPHA)) {
			png_set_filler(_png, 0xffff, PNG_FILLER_AFTER);
		}
	} else {
		png_set_strip_alpha(_png);
	}

// RGB <-> Grayscale
	auto out_color_model = axColorUtil::colorModel(out_color_type);
	switch (out_color_model) {
		case axColorModel::RGB:
		case axColorModel::RGBA: {
			if (!(in_color_type & PNG_COLOR_MASK_COLOR)) png_set_gray_to_rgb(_png);
		}break;

		case axColorModel::R:
		case axColorModel::L:
		case axColorModel::LA: {
			if (in_color_type & PNG_COLOR_MASK_COLOR) png_set_rgb_to_gray_fixed(_png, 1, -1, -1);
		}break;

		default: throw Error_ReadUnsupportedFormat();
	}
}

} //namespace
