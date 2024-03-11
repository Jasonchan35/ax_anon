#pragma once
#include "axString.h"
#include "axFormatParam.h"

class axFormat : public axNonCopyable {
public:
	struct Opt {
		// index [,alignment][ :formatString]  (same as C#)
		void set(axStrView8 param);

		axInt index = -1;
		axInt padding = 0;
		axInt precision = -1;
		bool  alignLeft = false;
		axString8 format;
	};
	Opt opt;
	axInt indent = 0;

	axFormat(axFormat* parent);
	virtual ~axFormat() {}
	
	template<class T> AX_INLINE
	axFormat& operator << (const T& s) & { _append(s); return *this; }

	template<class T, axInt N> AX_INLINE
	axFormat& operator << (const T (&sz)[N]) & { _append(sz); return *this; }
	
	template<class First, class... Args>
	void append(const First& first, Args&&... args) {
		_append(first);
		append(ax_forward(args)...);
	}

	void appendRepeat(axStrView8  v, axInt repeat) { onOutput(v, repeat); }
	void appendRepeat(axStrView16 v, axInt repeat) { onOutput(v, repeat); }
	void appendRepeat(axStrView32 v, axInt repeat) { onOutput(v, repeat); }
	void appendRepeat(axStrViewW  v, axInt repeat) { onOutput(v, repeat); }

	void appendRepeat(axChar8     v, axInt repeat) { onOutput(axStrView_char(v), repeat); }
	void appendRepeat(axChar16    v, axInt repeat) { onOutput(axStrView_char(v), repeat); }
	void appendRepeat(axChar32    v, axInt repeat) { onOutput(axStrView_char(v), repeat); }
	void appendRepeat(axCharW     v, axInt repeat) { onOutput(axStrView_char(v), repeat); }

	void appendQuoted(axStrView8  v);
	void appendQuoted(axStrView16 v);
	void appendQuoted(axStrView32 v);
	void appendQuoted(axStrViewW  v);

	void newline();

	template<class V>
	void reflection(const V& value);

	template<class FormatChar, axInt N>
	void	format(const FormatChar (&fmt)[N])	{
		format(axStrView_make<const FormatChar, N>(fmt));
	}

	template<class FormatChar, axInt N, class... Args>
	void	format(const FormatChar (&fmt)[N], const Args&... args)	{
		format(axStrView_make<const FormatChar, N>(fmt), args...);
	}

	template<class FormatChar>
	void	format(axStrViewT<FormatChar> fmt);

	template<class FormatChar, class... Args>
	void	format(axStrViewT<FormatChar> fmt, const Args&... args);

	virtual void onOutput(axStrView8  v, axInt repeat = 1) = 0;
	virtual void onOutput(axStrView16 v, axInt repeat = 1) = 0;
	virtual void onOutput(axStrView32 v, axInt repeat = 1) = 0;
	virtual void onOutput(axStrViewW  v, axInt repeat = 1) = 0;

	void onOutput(axChar8  v, axInt repeat = 1) { onOutput(axStrView_char(v), repeat); }
	void onOutput(axChar16 v, axInt repeat = 1) { onOutput(axStrView_char(v), repeat); }
	void onOutput(axChar32 v, axInt repeat = 1) { onOutput(axStrView_char(v), repeat); }
	void onOutput(axCharW  v, axInt repeat = 1) { onOutput(axStrView_char(v), repeat); }

	template<class T>
	void _doFormatT(axStrViewT<T> fmt, const axSpan<axFormatParam>& paramList);

	void _doFormat(axStrView8  fmt, const axSpan<axFormatParam>& paramList);
	void _doFormat(axStrView16 fmt, const axSpan<axFormatParam>& paramList);
	void _doFormat(axStrView32 fmt, const axSpan<axFormatParam>& paramList);
	void _doFormat(axStrViewW  fmt, const axSpan<axFormatParam>& paramList);

private:
	void append() {}
	void _append(axChar8  v) { onOutput(axStrView_char(v)); }
	void _append(axChar16 v) { onOutput(axStrView_char(v)); }
	void _append(axChar32 v) { onOutput(axStrView_char(v)); }
	void _append(axCharW  v) { onOutput(axStrView_char(v)); }

	void _append(axStrView8  v) { onOutput(v); }
	void _append(axStrView16 v) { onOutput(v); }
	void _append(axStrView32 v) { onOutput(v); }
	void _append(axStrViewW  v) { onOutput(v); }

	template<axInt N> void _append(const axChar8  (&v)[N]) { onOutput(v); }
	template<axInt N> void _append(const axChar16 (&v)[N]) { onOutput(v); }
	template<axInt N> void _append(const axChar32 (&v)[N]) { onOutput(v); }
	template<axInt N> void _append(const axCharW  (&v)[N]) { onOutput(v); }

	void _append(void const * v) { axFormatParam_handler_void_ptr(*this, v); }
	void _append(void * const v) { axFormatParam_handler_void_ptr(*this, v); }

	template<class T> AX_INLINE
	axFormat& _append(const T& value) & {
		axFormatParam_handler(*this, value);
		return *this;
	}

	template<class T>
	void _appendQuoted(const axStrViewT<T>&  v);
};

template<class T>
class axFormatterT : public axFormat {
	using Base = axFormat;
public:
	axFormatterT(axIStringT<T>& outStr, axFormat* parent = nullptr) 
		: Base(parent)
		, _outStr(outStr)
	{}

	virtual void onOutput(axStrView8  v, axInt repeat) override;
	virtual void onOutput(axStrView16 v, axInt repeat) override;
	virtual void onOutput(axStrView32 v, axInt repeat) override;
	virtual void onOutput(axStrViewW  v, axInt repeat) override;

private:
	axIStringT<T>& _outStr;
};

//------------------------

template<class FormatChar> inline
void axFormat::format(axStrViewT<FormatChar> fmt) {
	axTempString tmp;
	axFormatterT<FormatChar> formatter(tmp, this);

	formatter._doFormat(fmt, axSpan<axFormatParam>());
	onOutput(tmp, 1);
}

template<class FormatChar, class... Args> inline
void axFormat::format(axStrViewT<FormatChar> fmt, const Args&... args) {
	axTempString tmp;
	axFormatterT<FormatChar> formatter(tmp, this);

	axFormatParam list[] {ax_forward(args)...};
	formatter._doFormat(fmt, axSpan_make(list));
	onOutput(tmp, 1);
}

template<class T> inline
void axMutSpan<T>::onFormat(axFormat& f) const {
	f << "[";
	axInt i = 0;
	for(auto& e : *this) {
		if (i > 0) {
			f << ", ";
		}
		f << e;
		i++;
	}
	f << "]";
}

template<class T, axDeleteFunc<T> del, class LIST_NODE>
void axDList<T, del, LIST_NODE>::onFormat(axFormat& f) const {
	f << "[";
	axInt i = 0;
	for(auto& e : *this) {
		if (i > 0) {
			f << ", ";
		}
		f << e;
		i++;
	}
	f << "]";
}

template<class T> inline
void axPtrBase<T>::onFormat(axFormat& f) const {
	if (!_p) {
		f << "null";
	} else {
		f << *_p;
	}
}

template<class T> inline
void axRange_<T>::onFormat(axFormat& f) const {
	f.format("({?}+{?})", start, size);
}

template<class T>
template<class V> inline
axIStringT<T>& axIStringT<T>::convert(const V& v) & {
	clear();
	axFormatterT<T> f(*this);
	f << v;
	return *this;
}

template<class T>
template<class V> inline
axIStringT<T>& axIStringT<T>::appendConvert(const V& v) & {
	axFormatterT<T> f(*this);
	f << v;
	return *this;
}

template<class... ARGS> inline
void axTuple<ARGS...>::onFormat(axFormat& f) const {
	f << "Tuple[";
	forEach([&f](auto index, const auto& value){
		if (index > 0) f << ", ";
		f << value;
	});
	f << "]";
}

template<class V>
struct axFormat_ByReflection_Handler {
	const V& obj;
	axFormat& f;
	using R = axReflection<V>;

	template<axInt index, class Field>
	void handle() {
		const auto& v = Field::s_value(&obj);
		AX_IF_CONSTEXPR (R::isSingleRowLayout()) {
			if (index > 0) f << ", ";
			f << v;
		} else {
			f << "\n  " << Field::s_name() << " = " << v;
		}
	}
};

template<class V> inline
void axFormat::reflection(const V& obj) {
	using R = axReflection<V>;
	axFormat_ByReflection_Handler<V> h {obj, *this};
	AX_IF_CONSTEXPR (R::isSingleRowLayout()) {
		append("[");
		R::Fields::s_forEachType(h);
		append("]");
	} else {
		append("{");
		R::Fields::s_forEachType(h);
		append("}");
	}
}

#if AX_OS_MACOSX || AX_OS_IOS

template<> inline
void axFormatParam_handler(axFormat& f, NSString* s) {
	f << axStrView_make(s);
}

template<class T> inline
void axHashData<T>::onFormat(axFormat& f) const {
	f << _data;
}

#endif
