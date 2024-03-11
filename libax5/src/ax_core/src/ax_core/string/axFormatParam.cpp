#include <ax_core/string/axFormat.h>

class axFormatParam_NumberHandler {
public:
	axFormatParam_NumberHandler(axFormat& formatter_);

	template<class T> void handler( const T& value );

	template<class T> void to_digi_float(T value);
	template<class T> void to_digi_signed(T value);
	template<class T> void to_digi_unsigned(T value);

	#define AX_MACRO_OP(T) \
		void to_digi( T value ) { return to_digi_signed(value); } \
	//-----
		AX_TYPE_LIST_SINT(AX_MACRO_OP)
		AX_TYPE_LIST_EXTRA_NATIVE_SINT(AX_MACRO_OP);
	#undef AX_MACRO_OP

	#define AX_MACRO_OP(T) \
		void to_digi( T value ) { return to_digi_unsigned(value); } \
	//-----
		AX_TYPE_LIST_UINT(AX_MACRO_OP)
		AX_TYPE_LIST_EXTRA_NATIVE_UINT(AX_MACRO_OP);
	#undef AX_MACRO_OP

	#define AX_MACRO_OP(T) \
		void to_digi( T value ) { return to_digi_float(value); } \
	//-----
		AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
	#undef AX_MACRO_OP

	static const int kBufferSize = 128;

	axFormat& formatter;
	axFormat::Opt & opt;
	bool	plus_sign  :1;
	bool	exponent   :1;
	bool	fixedPoint :1;
	int		base;
	axChar	fill_ch;
	const axChar *hex_table;

	axTempString outBuf;
};

axFormatParam_NumberHandler::axFormatParam_NumberHandler(axFormat& formatter_)
	: formatter(formatter_)
	, opt(formatter_.opt)
	, plus_sign(false)
	, exponent(false)
	, fixedPoint(false)
	, base(10) //default is dec
	, fill_ch(' ')
	, hex_table(axCStr::kHexChars())
{
   
	//printf("to_str(int) value=%X opt=%ls\n", value, opt );
	auto fmt = opt.format.view();
	auto len = fmt.size();
	if( len ) {
		char last_ch = fmt.back();
		switch( last_ch ) {
            case 'c': { len--; base = 0;  } break; //character
            case 'b': { len--; base = 2;  } break; //binary
            case 'o': { len--; base = 8;  } break; //octer
            case 'd': { len--; base = 10; } break; //decmial
            case 'X': { len--; base = 16; } break; //hex
			case 'x': { len--; base = 16; hex_table = axCStr::kLowHexChars(); } break;  //lower HEX				
			case 'e': { len--; exponent = true; } break; //for floating point only
			case 'f': { len--; fixedPoint = true; } break;//for floating point only
		}
		
		const auto *p = fmt.data();;
		if (*p == '+') { 
			p++; 
			len--; 
			plus_sign = true; 
		}

		if (*p == '0') { 
			p++; 
			len--; 
			fill_ch = '0'; 
		}
	}
}

template<class T>
void axFormatParam_NumberHandler::handler(const T& value) {
	//printf("opt plus_sign=%s, padding=%d, ch_base=%c\n", plus_sign?"true":"false", padding, ch_base );
	axInt numSpace = 0;
    
	if( base == 0 ) {
		formatter.append(axCharU(value));
		plus_sign = false;
	}else{
		to_digi( value );
		if( ! axTypeTraits::isUnsigned<T>::value && ax_less_than0( value ) ) {
			plus_sign = false;
		}
	}
    
	axInt req_size = outBuf.size();
	if (plus_sign) req_size++;
	if (req_size < opt.padding) {
		numSpace = opt.padding - req_size;
	}
    
	if (plus_sign) {
		formatter.onOutput('+');
	}

	if (!opt.alignLeft) { //fill space at the front
		formatter.onOutput(fill_ch, numSpace);
	}
    
	formatter.onOutput(outBuf);

	if( opt.alignLeft ) { //fill space at the end
		formatter.onOutput(fill_ch, numSpace);
	}
    
	//printf("\n str = [%ls] \n\n", str.sz() );
}

template< class T> inline
void axFormatParam_NumberHandler::to_digi_signed( T value ) {
	T v = value;
	T b = (T)base;

	axInt n;
	for( n=0; n<kBufferSize; ) {
		v /= b;
		n++;
		if( v == 0 ) break;
	}
	if( value < 0 ) n++;
	if( n >= kBufferSize ) {
		throw axStringError_FormatNumberHandler();
	}

	outBuf.resize(n);
	auto* out = outBuf.data() + n - 1;

	if( value < 0 ) outBuf[0] = '-';

	v = value;
	for( axInt i=0; i<n; ) {
		T a = v % b;
		*out = (value < 0) ? hex_table[-a] : hex_table[a];
		out--;
		v /= b;
		i++;
		if( v == 0 ) break;
	}

}

template< class T> inline
void axFormatParam_NumberHandler::to_digi_unsigned( T value ) {
	T v = value;
	T b = (T)base;
	axInt i;

	for( i=0; i<kBufferSize; ) {
		v /= b;
		i++;
		if( v == 0 ) break;
	}

	outBuf.resize(i);
	auto* out = outBuf.data() + i - 1;

	v = value;
	for( i=0; i<kBufferSize; ) {
		T a = v % b;
		*out = hex_table[a];
		out--;
		v /= b;
		i++;
		if( v == 0 ) break;
	}
}

template< class T> inline
void axFormatParam_NumberHandler::to_digi_float( T value ) {
	if( base != 10 ) {
		throw axStringError_FormatNumberHandler();
	}

	char tmp[64 + 1];
	tmp[64] = 0;
	char type = exponent ? 'e' : ( fixedPoint ? 'f' : 'g');

	outBuf.resize(kBufferSize);
	int precision = (int)opt.precision;

#if AX_COMPILER_VC
	if( precision < 0 ) {
		_snprintf( tmp, 64, "%%%c", type );
	}else{
		_snprintf( tmp, 64, "%%.%d%c", precision, type );
	}
	int ret = _snprintf( outBuf.data(), kBufferSize, tmp, value );

#else
	if( precision < 0 ) {
		snprintf( tmp, 64, "%%%c", type );
	}else{
		snprintf( tmp, 64, "%%.%d%c", precision, type );
	}
	int ret = snprintf( outBuf.data(), kBufferSize, tmp, value );

#endif
	if (ret < 0 || ret >= kBufferSize) {
		throw axStringError_FormatNumberHandler();
	}
	outBuf.resize(ret);
}

template<class T> AX_INLINE
void axFormatParam_CharHandler(axFormat& f, const T& value) {
	if (f.opt.padding > 0) f.onOutput(' ',  f.opt.padding - 1);
	f.onOutput(axStrView_char(value));
	if (f.opt.padding < 0) f.onOutput(' ', -f.opt.padding - 1);
	return;
}

#define AX_MACRO_OP(T) \
	template<> void axFormatParam_handler(axFormat& f, const T& value) { axFormatParam_CharHandler<T>(f, value); } \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP);
#undef AX_MACRO_OP

#define AX_MACRO_OP(T) \
	template<> \
	void axFormatParam_handler<T>(axFormat& f, const T& value) { \
		axFormatParam_NumberHandler h(f); \
		h.handler(value); \
	} \
//--------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
	AX_TYPE_LIST_EXTRA_NATIVE_SINT(AX_MACRO_OP);
	AX_TYPE_LIST_EXTRA_NATIVE_UINT(AX_MACRO_OP);
#undef AX_MACRO_OP

void axFormatParam_handler_void_ptr(axFormat& f, void const* value) {
	auto int_value = axPtrUtil::toUInt(value);
	f.opt.format = "0X";
	axFormatParam_NumberHandler h(f);
	ax_max_it(f.opt.padding, ax_sizeof(value) * 2);
	h.handler(int_value);
}

template<>
void axFormatParam_handler<bool>(axFormat& f, const bool& value) {
	f.append(axStrLiteral_bool(value));
}

template<>
void axFormatParam_handler<ax_exception>(axFormat& f, const ax_exception& e) {
	return axFormatParam_handler(f, axStrView_c_str(e.what()));
}

template<>
void axFormatParam_handler<axHash>(axFormat& f, const axHash& v) {
	f << v.value;
}

#if AX_OS_MACOSX || AX_OS_IOS

void axFormatParam_handler_NSError(axFormat& f, const NSError* error) {
	if (!error) {
		f.append("nil");
		return;
	}

	f.format("code={?}, domain={?}\n", error.code, error.domain);
	if (error.userInfo) {
		for (NSErrorUserInfoKey key in error.userInfo) {
			f.format("  - {?}=", key);

			id v = error.userInfo[key];
			if ([v isKindOfClass:[NSString class]]) {
				f.append((NSString*)v);
			}

			f.append("\n");
		}
	}
}

template<>
void axFormatParam_handler<NSError>(axFormat& f, NSError const & error) {
	axFormatParam_handler_NSError(f, &error);
}

template<>
void axFormatParam_handler<NSError*>(axFormat& f, NSError* __strong const & error) {
	axFormatParam_handler_NSError(f, error);
}

template<>
void axFormatParam_handler<NSString*>(axFormat& f, NSString* __strong const & v) {
	if (!v) {
		f.append("nil");
		return;
	}
	axTempString tmp;
	tmp.setUtf_c_str([v UTF8String]);
	f.append(tmp);
}

#endif
