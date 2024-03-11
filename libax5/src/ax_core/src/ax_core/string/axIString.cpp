#include <ax_core/string/axFormat.h>
#include <ax_core/string/axUtfUtil.h>

template<class T>
void	axIStringT<T>::clearAndFree() {
	clear();
	if (!_storage.isSmall()) {
		if (auto* p = _storage.data()) {
			onFree(p);
		}
		_storage.reset();
	}
}

template< class T >
void	axIStringT<T>::_reserve(axInt newSize) {
	auto buf = onMalloc(newSize + 1); // +1 for the null terminator

	auto* oldData = _storage.data();
	auto  oldSize = _storage.size();

	if (buf.data() != oldData) {
		if (oldSize) {
			ax_memcpy(buf.data(), oldData, (oldSize + 1) * ax_sizeof(T)); // +1 for the null terminator
		}
		if (!_storage.isSmall() && oldData) {
			onFree(oldData);
		}
	}

	if (buf.size() <= 0) {
		AX_ASSERT(false);
		_storage.reset();
		throw axError_Undefined(AX_LOC);
	}

	_storage.setData(buf.data(), buf.size() - 1);  // -1 for the null terminator
}

template< class T > inline
void	axIStringT<T>::_append (View view, axInt repeat) {
	auto old_size = size();
	auto total_size = view.size() * repeat;

	resize( old_size + total_size );
	auto* dst = data() + old_size;
	for( axInt i=0; i<repeat; i++ ) {
		ax_memcpy(dst, view.data(), view.sizeInBytes());
		dst += view.size();
	}
}

//-----------
template< class T >
template< class R >
void	axIStringT<T>::_appendUtf(axStrViewT<R> src, axInt repeat) {
	if (axArrayUtil::isOverlapped(this->data(), capacity(), src.data(), src.size())) {
		throw axError_BufferOverlapped();
		return;
	}

	if( repeat  <= 0 ) return;
	if( src.size() <= 0 ) return;

	auto old_size = size();
 	reserve(old_size + src.size() * repeat);

	try{
		// convert first
		axUtfUtil::convert(*this, src);
		auto converted_size = size() - old_size;

		resize(old_size + converted_size * repeat);
		// repeat from the first
		auto p0 = data() + old_size;
		auto dst = p0 + converted_size;
		for( axInt i=1; i<repeat; ++i ) {
			axArrayUtil::copy( dst, p0, converted_size );
			dst += converted_size;
		}
	}catch(...){
		resize( old_size );
		throw;
	}
}

template<class T>
template<class FormatChar> inline
void axIStringT<T>::_appendFormatWithParamList(axStrMutViewT<const FormatChar> fmt, const axSpan<FormatParam>& paramList) {
	axFormatterT<T> formatter(*this);
	formatter._doFormat(fmt, paramList);
}

template<class T>
void axIStringT<T>::appendFormatWithParamList(axStrView8 fmt, const axSpan<FormatParam>& paramList) {
	_appendFormatWithParamList(fmt, paramList);
}

template<class T>
void axIStringT<T>::appendFormatWithParamList(axStrView16 fmt, const axSpan<FormatParam>& paramList) {
	_appendFormatWithParamList(fmt, paramList);
}

template<class T>
void axIStringT<T>::appendFormatWithParamList(axStrView32 fmt, const axSpan<FormatParam>& paramList) {
	_appendFormatWithParamList(fmt, paramList);
}

template<class T>
void axIStringT<T>::appendFormatWithParamList(axStrViewW fmt, const axSpan<FormatParam>& paramList) {
	_appendFormatWithParamList(fmt, paramList);
}

template<class T> inline axIStringT<T>& axIStringT<T>::appendUtfRepeat(axStrView8  v, axInt repeat) & { _appendUtf(v, repeat); return *this; } \
template<class T> inline axIStringT<T>& axIStringT<T>::appendUtfRepeat(axStrView16 v, axInt repeat) & { _appendUtf(v, repeat); return *this; } \
template<class T> inline axIStringT<T>& axIStringT<T>::appendUtfRepeat(axStrView32 v, axInt repeat) & { _appendUtf(v, repeat); return *this; } \
template<class T> inline axIStringT<T>& axIStringT<T>::appendUtfRepeat(axStrViewW  v, axInt repeat) & { _appendUtf(v, repeat); return *this; } \

//------------

template< class T >
void axIStringT<T>::replaceChars(T from, T to) {
	auto* s = data();
	auto* e = s + size();
	for ( ; s < e; s++ ) {
		if (*s == from) *s = to;
	}
}

template< class T >
axInt axIStringT<T>::replaceAll(View from, View to, bool ignoreCase) {
	if (!from) return 0;

	axTempStringT<T> tmpStr(*this);
	clear();

	axInt replaced = 0;

	auto v = tmpStr.view();

	while (v) {
		auto idx = v.find(from, ignoreCase);
		if (idx < 0) break;

		append(v.slice(0, idx));
		append(to);
		v = v.sliceFrom(idx + from.size());
		replaced++;
	}

	append(v);

	return replaced;
}

template<class T>
void axIStringT<T>::insert(axInt pos, View text) {
	axTempStringT<T> tmp;
	tmp.reserve(size() + text.size());
	tmp.append(slice(0, pos), text, sliceFrom(pos));
	set(tmp);
}

template< class T >
void axIStringT<T>::removeChars(axInt pos, axInt count) {
	axTempStringT<T> tmp;
	axInt s = pos + count;
	if (s > size()) return;

	tmp.append(slice(0, pos), sliceFrom(pos + count));
	set(tmp);
}

template<class T>
void axIStringT<T>::appendHex(axByteSpan src, bool upperCase, T separator) {
	auto* hex = upperCase ? axCStr::kHexChars() : axCStr::kLowHexChars();

	axInt n = separator ? 3 : 2;

	axInt oldSize = size();
	axInt newSize = oldSize + src.size() * n;

	resize(newSize);
	auto* dst = data() + oldSize;
	for (auto c : src) {
		*dst = hex[(c >> 4) & 0xf]; dst++;
		*dst = hex[ c       & 0xf]; dst++;
		if (separator) {
			*dst = separator; dst++;
		}
	}
}

//------------

#define	AX_MACRO_OP(T)	\
	AX_RTTI_INTERNAL_CLASS_IMPL(template<>, axIStringT<T>) \
	/*---- The explicit instantiation ---*/ \
	template class axIStringT<T>; \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef	AX_MACRO_OP



