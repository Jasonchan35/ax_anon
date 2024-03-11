#pragma once

#if !AX_BUILD_ax_render
	#error "only include this file when building ax_render"
#endif

#include <ax_render/image/axImage.h>
#include <setjmp.h>

extern "C" {
	#include <jpeglib.h>
}

namespace axImageIO_jpeg {

class Error : public axError {};
class Error_StartDecompress			: public Error {};
class Error_FinishDecompress		: public Error {};
class Error_ReadUnsupportedFormat	: public Error {};
class Error_ReadHeader				: public Error {};
class Error_ReadPixels				: public Error {};

class Reader {
public:
	Reader();
	~Reader();

	void load(axImage& img, axByteSpan data, axColorType expectType = axColorType::Unknown);

private:
	static void		s_init_source(j_decompress_ptr cinfo) {}
	static boolean	s_fill_input_buffer(j_decompress_ptr cinfo);
	static void		s_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
	static void		s_term_source(j_decompress_ptr cinfo) {}
	static void		s_output_message(j_common_ptr cinfo);
	static void		s_error_exit(j_common_ptr cinfo);

	bool error_exit_longjmp_restore_point();

	jpeg_decompress_struct _cinfo;
	axByteSpan _data;
	axByteSpan _remain;

	jpeg_source_mgr _srcMgr;
	jpeg_error_mgr _errMgr;

#if AX_COMPILER_VC
	#pragma warning(push) 
	#pragma warning(disable: 4324) // structure was padded due to alignment specifier
#endif
	jmp_buf _setjmp_buffer;
#if AX_COMPILER_VC
	#pragma warning(pop) 
#endif
};

} //namespace

