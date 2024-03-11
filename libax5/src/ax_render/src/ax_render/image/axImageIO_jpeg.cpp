#include "axImageIO_jpeg.h"

namespace axImageIO_jpeg {

void Reader::s_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];
	cinfo->err->format_message(cinfo, buffer);
	AX_LOG("jpeg error {?}", buffer);
}

void Reader::s_error_exit (j_common_ptr cinfo) {
	s_output_message(cinfo);
	cinfo->err->output_message(cinfo);
	auto* p = static_cast<Reader*>(cinfo->client_data);
	longjmp(p->_setjmp_buffer, 1);
}

bool Reader::error_exit_longjmp_restore_point() {
	// !!! call this function before any libjpeg C-function that might longjmp()
	// to avoid any C++ destructor or exception try/catch block happen in between

	#if AX_COMPILER_VC
		#pragma warning(push) 
		#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	#endif

	// longjmp() to here from error_exit()
	return setjmp(_setjmp_buffer) != 0;

	#if AX_COMPILER_VC
		#pragma warning(pop) 
	#endif
}

boolean Reader::s_fill_input_buffer (j_decompress_ptr cinfo) {
	AX_LOG("jpeg error fill_input_buffer");
	return FALSE;
}

void Reader::s_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	if (num_bytes > 0) {
		cinfo->src->next_input_byte += (size_t) num_bytes;
		cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
	}
}

Reader::Reader() {
	ax_bzero_struct(_cinfo);
	ax_bzero_struct(_errMgr);
	ax_bzero_struct(_srcMgr);
}

Reader::~Reader() {
	jpeg_destroy_decompress(&_cinfo);
}

void Reader::load(axImage& img, axByteSpan data, axColorType expectType) {
	_cinfo.err = jpeg_std_error(&_errMgr);
	_errMgr.output_message = s_output_message;
	_errMgr.error_exit = s_error_exit;

	jpeg_create_decompress(&_cinfo);

	_srcMgr.init_source = s_init_source;
	_srcMgr.fill_input_buffer = s_fill_input_buffer;
	_srcMgr.skip_input_data = s_skip_input_data;
	_srcMgr.resync_to_restart = jpeg_resync_to_restart; // use default method
	_srcMgr.term_source = s_term_source;

	_srcMgr.bytes_in_buffer = static_cast<size_t>(data.size());
	_srcMgr.next_input_byte = (JOCTET*)data.data();

	_cinfo.src = &_srcMgr;

	//-------
	if (error_exit_longjmp_restore_point()) {
		throw Error_ReadHeader();
	}
	if (jpeg_read_header(&_cinfo, TRUE) != JPEG_HEADER_OK) {
		throw Error_ReadHeader();
	}

	//-------
	if (error_exit_longjmp_restore_point()) {
		throw Error_StartDecompress();
	}
	if (jpeg_start_decompress(&_cinfo) != TRUE) {
		throw Error_StartDecompress();
	}
	
	_cinfo.out_color_space = JCS_RGB;

	axInt width = 0;
	axInt height = 0;
	ax_safe_assign(width,  _cinfo.output_width);
	ax_safe_assign(height, _cinfo.output_height);

	if (width <= 0 || height <= 0) {
		throw Error_ReadHeader();
	}

	int srcPixelSize = _cinfo.output_components;

	auto out_color_type = axColorType::Unknown;
	auto src_color_type = axColorType::Unknown;

	switch (srcPixelSize) {
		case 1: src_color_type = axColorType::Lb;   break;
		case 3: src_color_type = axColorType::RGBb; break;
	}

	if (expectType == axColorType::Unknown) {
		switch (srcPixelSize) {
			case 1:	out_color_type = axColorType::Lb;		break;
			case 3:	out_color_type = axColorType::RGBAb;	break; //DirectX doesn't support RGBb, so convert to RGBAb instead
		}
	}

	if (src_color_type == axColorType::Unknown || out_color_type == axColorType::Unknown) {
		throw Error_ReadUnsupportedFormat();
	}

	auto srcRowStride = width * srcPixelSize;
	img.create(out_color_type, width, height);
		
	if (out_color_type == expectType) { 
		// no conversion is needed, decode directly to image buffer
		for (axInt y = 0; y < height; y++) {
			JSAMPLE* scanline[1] = {
				reinterpret_cast<JSAMPLE*>(img.rowBuffer(y).data())
			};

			//-------
			if (error_exit_longjmp_restore_point()) {
				throw Error_ReadPixels();
			}
			if (jpeg_read_scanlines(&_cinfo, scanline, 1) < 1) {
				throw Error_ReadPixels();
			}
		}
	} else {
		axByteArray_<2048 * 4> srcRow;
		srcRow.resize(srcRowStride);
		JSAMPLE* scanline[1] = {
			reinterpret_cast<JSAMPLE*>(srcRow.data())
		};

		for (axInt y = 0; y < height; y++) {
			//-------
			if (error_exit_longjmp_restore_point()) {
				throw Error_ReadPixels();
			}
			if (jpeg_read_scanlines(&_cinfo, scanline, 1) < 1) {
				throw Error_ReadPixels();
			}

			auto outRow = img.rowBuffer(y);
			axColorUtil::convertSpanByType(out_color_type, outRow, src_color_type, srcRow);
		}
	}

	if (jpeg_finish_decompress(&_cinfo) != TRUE) {
		throw Error_FinishDecompress();
	}
}


} //namespace
