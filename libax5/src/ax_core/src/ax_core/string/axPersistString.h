#pragma once

#include "../pointer/axSPtr.h"
#include "../container/axMap.h"
#include "../thread/axSpinLock.h"
#include "axStrLiteral.h"
#include "axFormat.h"

template<class T>
class axPersistStringT { // copyable
	using Base = axStrLiteralT<T>;
	using This = axPersistStringT;
public:
	using StrView	 = axStrViewT<const T>;
	using StrLiteral = axStrLiteralT<T>;

	static axPersistStringT	s_make(StrView s);

	const StrLiteral& str() const { return *_str; }

	operator const StrLiteral&	() const { return str(); }
	operator const StrView&		() const { return str(); }

	bool operator==(const This& r) const { return _str == r._str; }
	bool operator!=(const This& r) const { return _str != r._str; }

	bool operator==(StrView v) const { return *_str == v; }
	bool operator!=(StrView v) const { return *_str == v; }

	axPersistStringT() : _str(s_empty()) {}

	void onFormat(axFormat& f) const { f << str(); }

class Manager;
friend class Manager;
protected:
	axPersistStringT(const StrLiteral* str) : _str(str) {}
	static	const StrLiteral*	s_empty();

private:

	const StrLiteral*	_str = nullptr;
};

using axPersistString   = axPersistStringT<axChar>;
using axPersistStringW  = axPersistStringT<axCharW>;
using axPersistString8  = axPersistStringT<axChar8>;
using axPersistString16 = axPersistStringT<axChar16>;
using axPersistString32 = axPersistStringT<axChar32>;
