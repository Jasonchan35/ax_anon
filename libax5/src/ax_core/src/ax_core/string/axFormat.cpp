#include <ax_core/string/axFormat.h>

axFormat::axFormat(axFormat* parent) {
	if (parent) {
		indent = parent->indent;
	}
}

template<class T> AX_INLINE
void axFormat::_appendQuoted(const axStrViewT<T>& v) {
	_append('\"');
	for (auto ch : v) {
		switch (ch) {
			case '\"':	_append("\\\"");	break;
			case '\\':	_append("\\\\");	break;
			default:	_append(ch);		break;
		}
	}
	_append('\"');
}

void axFormat::appendQuoted(axStrView8  v) { _appendQuoted(v); }
void axFormat::appendQuoted(axStrView16 v) { _appendQuoted(v); }
void axFormat::appendQuoted(axStrView32 v) { _appendQuoted(v); }
void axFormat::appendQuoted(axStrViewW  v) { _appendQuoted(v); }

void axFormat::newline() {
	_append('\n');
	appendRepeat("  ", indent);
}

void axFormat::Opt::set(axStrView8 param) {
	index = -1;
	padding = 0;
	precision = -1;

	auto pair = param.splitByChar(':');
	auto param_other = pair.second;
	format.set(param_other);

	auto param_index_and_space = pair.first.splitByChar(',');
	auto param_index = param_index_and_space.first;
	auto param_space = param_index_and_space.second;

	if (param_index == "?") {

	} else if (param_index && !param_index.tryParse(index)) {
		throw axStringError_InvalidParamIndexInFormat();
	}

	// padding and precision
	if (param_space.size()) {
		auto param_space_pair = param_space.splitByChar('.');
		auto param_space_padding   = param_space_pair.first;
		auto param_space_precision = param_space_pair.second;

		param_space_padding.tryParse(padding);

		alignLeft = padding < 0;
		padding = ax_abs(padding);

		if (param_space_precision.size()) {
			param_space_precision.tryParse(precision);
		}
	}
}

template<class T>
void axFormat::_doFormatT(axStrViewT<T> fmt, const axSpan<axFormatParam>& paramList) {
	//find '{' and '}'
	const T* s = nullptr; //start
	const T* e = nullptr; //end
	const T* c = fmt.data();
	const T* raw = fmt.data();

	axInt cur_index = 0;

	for( ; *c; c++ ) {
		switch( *c ) {
			case '{': {
				if( c[1] == '{' ) {
					c++;
				}else{
					s=c;
				}

				onOutput(axStrView_make( raw, c-raw) );
				raw = c+1;
			}break;

			case '}': {
				raw = c+1;
				if( !s ) { //found '}' before '{'
					onOutput(*c);
					continue;
				}
				e=c;

				s++;

				axTempString8 param;
				param.setUtf(axStrView_make(s, e-s));
				opt.set(param);

				if (opt.index < 0) {
					opt.index = cur_index;
				}

				cur_index++;
				if (!paramList.inBound(opt.index )) {
					// out "{?} directly"
					onOutput( axStrView_make( s-1, e-s+2 ) );
					throw axStringError_InvalidParamIndexInFormat();
				}else{
					auto& pa = paramList[opt.index];
					pa.onProc(*this);
				}
				s = nullptr;
			}break;
		}
	}

	if (s != nullptr) {
		throw axStringError_FormatCurlyBracesMismatch(s - fmt.data());
	}

	onOutput( axStrView_make( raw, c-raw ) );
}

void axFormat::_doFormat(axStrView8  fmt, const axSpan<axFormatParam>& paramList) { _doFormatT(fmt, paramList); }
void axFormat::_doFormat(axStrView16 fmt, const axSpan<axFormatParam>& paramList) { _doFormatT(fmt, paramList); }
void axFormat::_doFormat(axStrView32 fmt, const axSpan<axFormatParam>& paramList) { _doFormatT(fmt, paramList); }
void axFormat::_doFormat(axStrViewW  fmt, const axSpan<axFormatParam>& paramList) { _doFormatT(fmt, paramList); }

//-----------------------

template<class T> inline void axFormatterT<T>::onOutput(axStrView8  v, axInt repeat) { _outStr.appendUtfRepeat(v, repeat); }
template<class T> inline void axFormatterT<T>::onOutput(axStrView16 v, axInt repeat) { _outStr.appendUtfRepeat(v, repeat); }
template<class T> inline void axFormatterT<T>::onOutput(axStrView32 v, axInt repeat) { _outStr.appendUtfRepeat(v, repeat); }
template<class T> inline void axFormatterT<T>::onOutput(axStrViewW  v, axInt repeat) { _outStr.appendUtfRepeat(v, repeat); }

#define	AX_MACRO_OP(T)	\
	template class axFormatterT<T>; \
//-------------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP);
#undef AX_MACRO_OP
