#include <ax_core/string/axFormat.h>

template<class T>
axInt axStrMutViewT<T>::compare(View r, bool ignoreCase) const {
	if (data() == r.data() && size() == r.size()) {
		return true;
	}
	axInt n = ax_min(size(), r.size());
	axInt ret;
	if (ignoreCase) {
		ret = axCStr::caseCompare(data(), r.data(), n);
	}else{
		ret = axCStr::compare(data(), r.data(), n);
	}

	if (ret != 0) return ret;

	if (size() < r.size()) {
		return -1;
	}else if (size() > r.size()){
		return 1;
	}
	return 0;
}

template<class T>
axInt axStrMutViewT<T>::findChar(T ch, bool ignoreCase) const {
	auto* start = _data;
	auto* end = start + _size;
	auto* p = start;

	if (ignoreCase) {
		for ( ; p < end; p++ ) {
			if (axCStr::charCaseCompare(*p, ch) == 0) {
				return p - _data;
			}
		}
	} else {
		for ( ; p < end; p++ ) {
			if (*p == ch) {
				return p - _data;
			}
		}
	}
	return -1;
}

template<class T>
axInt axStrMutViewT<T>::findCharBack(T ch, bool ignoreCase) const {
	if (size() <= 0) return -1;
	auto* start = _data;
	auto* end = start + _size;
	auto* p = end - 1;

	if (ignoreCase) {
		for ( ; p >= start; p-- ) {
			if (axCStr::charCaseCompare(*p, ch) == 0) {
				return p - start;
			}
		}
	} else {
		for ( ; p >= start; p-- ) {
			if (*p == ch) {
				return p - start;
			}
		}
	}
	return -1;
}

template<class T>
axInt axStrMutViewT<T>::findChars(View list, bool ignoreCase) const {
	auto* start = _data;
	auto* end = start + _size;
	auto* p = start;

	if (ignoreCase) {
		for ( ; p < end; p++ ) {
			for (auto& ch : list) {
				if (axCStr::charCaseCompare(*p, ch) == 0) {
					return p - _data;
				}
			}
		}
	} else {
		for ( ; p < end; p++ ) {
			for (auto& ch : list) {
				if (*p == ch) {
					return p - _data;
				}
			}
		}
	}
	return -1;
}

template<class T>
axInt axStrMutViewT<T>::findCharsBack(View list, bool ignoreCase) const {
	if (_size <= 0) return -1;
	auto* start = _data;
	auto* end = start + _size;
	auto* p = end - 1;

	if (ignoreCase) {
		for ( ; p >= start; p-- ) {
			for(auto& ch : list) {
				if (axCStr::charCaseCompare(*p, ch) == 0) {
					return p - start;
				}
			}
		}
	} else {
		for ( ; p >= start; p-- ) {
			for(auto& ch : list) {
				if (*p == ch) {
					return p - start;
				}
			}
		}
	}
	return -1;
}

template<class T>
axInt axStrMutViewT<T>::find(View str, bool ignoreCase) const {
	auto n = str.size();
	if (n > _size) return -1;

	auto* start = _data;
	auto* end = start + _size - n + 1;
	auto* p = start;

	if (ignoreCase) {
		for ( ; p < end; p++ ) {
			if (0 == axCStr::caseCompare(p, str.data(), n)) {
				return p - _data;
			}
		}
	}else{
		for ( ; p < end; p++ ) {
			if (0 == axCStr::compare(p, str.data(), n)) {
				return p - _data;
			}
		}
	}
	return -1;
}

template<class T>
axInt axStrMutViewT<T>::findBack(View str, bool ignoreCase) const {
	auto n = str.size();
	if (n > _size) return -1;

	auto* start = _data;
	auto* end = start + _size - n;
	auto* p = end - 1;

	if (ignoreCase) {
		for ( ; p >= start; p-- ) {
			if (axCStr::caseEquals(p, str.data(), n)) {
				return p - _data;
			}
		}
	}else{
		for ( ; p >= start; p-- ) {
			if (axCStr::equals(p, str.data(), n)) {
				return p - _data;
			}
		}
	}
	return -1;
}

template< class T >
axStrMutViewT<T> axStrMutViewT<T>::trim(T ch) {
	auto* s = _data;
	auto* e = _data + _size;
	for (;;) {
		if (s >= e) return s_empty();
		if (*s != ch) break;
		s++;
	}
	return This(s, e - s);
}

template< class T >
axStrMutViewT<T> axStrMutViewT<T>::trimBack(T ch) {
	if (!_size) return s_empty();

	auto* s = _data;
	auto* e = _data + _size;
	for (;;) {
		if (s >= e) return s_empty();
		e--;
		if (*e != ch) break;
	}
	return This(s, e - s + 1);
}

template< class T >
axStrMutViewT<T> axStrMutViewT<T>::trimCharsBack(View charList) {
	if (!_size) return s_empty();

	auto* s = _data;
	auto* e = _data + _size;
	for (;;) {
		if (s >= e) return s_empty();
		e--;
		if (charList.findChar(*e) < 0) break;
	}
	return This(s, e - s + 1);
}

template< class T >
axStrMutViewT<T> axStrMutViewT<T>::trimChars(View charList) {
	auto* s = _data;
	auto* e = _data + _size;
	for (;;) {
		if (s >= e) return s_empty();
		if (charList.findChar(*s) < 0) break;
		s++;
	}
	return This(s, e - s);
}

template<class T>
axHash axStrMutViewT<T>::onGetHash() const {

	//"djb2" string hash
	//this algorithm (k=33) was first reported by dan bernstein many years ago
	//in comp.lang.c. another version of this algorithm (now favored by bernstein)
	//uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33
	//(why it works better than many other constants, prime or not) has never
	//been adequately explained.

    axHash h = axHash_get(5381);
    axHash::Value c;
	const T* s = data();
	const T* e = s + _size;
	for (;s<e; s++) {
		c = static_cast<axHash::Value>(*s);
        h.value = ((h.value << 5) + h.value) + c; // h * 33 + c
	}
	return h;
}

template<class T>
void axStrMutViewT<T>::onFormat(axFormat & f) const { 
	axInt padding = f.opt.padding - _size;
	if (padding < 0) {
		padding = 0;
	}

	if (padding && f.opt.alignLeft) {
		f.onOutput(' ', padding);
	}

	f.onOutput(*this); 

	if (padding && !f.opt.alignLeft) {
		f.onOutput(' ', padding);
	}
}

template<class T>
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitByIndex(axInt index) {
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	if (index >= _size) {
		return axPair_make(s_empty(), *this);
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template<class T>
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitByChar(T ch, bool ignoreCase) {
	auto index = findChar(ch, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template<class T>
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitByCharBack(T ch, bool ignoreCase) {
	auto index = findCharBack(ch, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template< class T >
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitByChars(View list, bool ignoreCase) {
	auto index = findChars(list, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template< class T >
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitByCharsBack(View list, bool ignoreCase) {
	auto index = findCharsBack(list, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template<class T>
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::split(View str, bool ignoreCase) {
	auto index = find(str, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + str.size()));
}

template<class T>
axPair<axStrMutViewT<T>, axStrMutViewT<T>> axStrMutViewT<T>::splitBack(View str, bool ignoreCase) {
	auto index = findBack(str, ignoreCase);
	if (index < 0) {
		return axPair_make(*this, s_empty());
	}
	return axPair_make(slice(0, index), sliceFrom(index + 1));
}

template< class T >
void axStrMutViewT<T>::getTokens(axIArray<MutView> & outList, T separator, bool ignoreCase) {
	outList.clear();
	auto tmp = *this;
	while (tmp) {
		auto pair = tmp.splitByChar(separator, ignoreCase);
		outList.append(pair.first);	
		tmp = pair.second;
	}
}

template< class T >
bool axStrMutViewT<T>::matchWildcard(View wildcard, bool ignoreCase) const {
	const auto* p = begin();
	const auto* e = end();

	const auto* w = wildcard.begin();
	const auto* wEnd = wildcard.end();

	while (p < e && w < wEnd) {
		if (*w == '?') {
			p++;
			w++;
			continue;
		}

		if (ignoreCase) {
			if (axCStr::charCaseEquals(*w, *p)) {
				p++;
				w++;
				continue;
			}
		}else{
			if (*w == *p) {
				p++;
				w++;
				continue;
			}
		}
		
		if (*w == '*') {
			auto w1 = w + 1;
			if (w1 >= wEnd) return true; // * is the last

			auto p1 = p + 1;
			if (p1 >= e) return false;

			p = p1;
			if (*p1 == *w1) {
				w = w1; //next wildcard
			}

			continue;
		}
		
		return false;
	}

	if (p == e && w == wEnd) return true;

	return false;
}

//-----------
#define	AX_MACRO_OP(T)	\
	AX_RTTI_INTERNAL_STRUCT_IMPL(template<>, axStrMutViewT<T>) \
	AX_RTTI_INTERNAL_STRUCT_IMPL(template<>, axStrMutViewT<const T>) \
	/*---- The explicit instantiation ---*/ \
	template class axStrMutViewT<T>; \
	template class axStrMutViewT<const T>; \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef	AX_MACRO_OP
