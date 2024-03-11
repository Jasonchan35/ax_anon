//
//  axEnvVar.cpp
//  libax_core
//
//  Created by Jason on 25/4/2020.
//

#include "axEnvVar.h"
#include "../string/axString.h"

namespace axEnvVar {

bool putValue(axStrView string) {
#if AX_OS_WINDOWS
	axTempStringW tmp; tmp.setUtf(string);
	int ret = ::_wputenv(tmp.c_str());
	return ret == 0;
#else
	axTempString tmp(string);
	int ret = ::putenv(ax_const_cast(tmp.c_str()));
	return ret == 0;
#endif
}

bool setValue(axStrView name, axStrView value, bool overwrite) {
#if AX_OS_WINDOWS
	axTempStringW tmpName;  tmpName.setUtf(name);
	axTempStringW tmpValue; tmpValue.setUtf(value);

	if (!overwrite) {
		wchar_t buf[8];
		if (0 == ::GetEnvironmentVariable(tmpName.c_str(), buf, 1))
			return false; // not found
	}

	int ret = ::SetEnvironmentVariable(tmpName.c_str(), tmpValue.c_str());
	return ret;

#else
	axTempString tmpName(name);
	axTempString tmpValue(value);
	int ret = ::setenv(tmpName.c_str(), tmpValue.c_str(), overwrite ? 1:0);
	return ret == 0;
#endif
}

bool unsetValue(axStrView name) {
#if AX_OS_WINDOWS
	axTempStringW tmpName;  tmpName.setUtf(name);
	
	int ret = ::SetEnvironmentVariable(tmpName.c_str(), nullptr);
	return ret;

#else
	axTempString tmpName(name);
	int ret = ::unsetenv(tmpName.c_str());
	return ret == 0;
#endif
}

bool getValue(axIString& outValue, axStrView name) {
#if AX_OS_WINDOWS
	outValue.clear();

	axTempStringW tmpName;  tmpName.setUtf(name);

	axFixedArray<wchar_t, 256> smallBuf;
	auto ret = ::GetEnvironmentVariable(tmpName.c_str(), smallBuf.data(), static_cast<DWORD>(smallBuf.size()));
	if (ret == 0) return false;

	if (ret < smallBuf.size()) {
		outValue.setUtf(axStrViewW(smallBuf.data(), ret));
		return true;
	}

	axTempStringW largeBuf;
	largeBuf.resize(ret);

	ret = ::GetEnvironmentVariable(tmpName.c_str(), largeBuf.data(), static_cast<DWORD>(largeBuf.size()));
	if (ret == 0) return false;
	if (ret == largeBuf.size()) return false;

	outValue.setUtf(axStrViewW(largeBuf.data(), ret));
	return true;

#else
	outValue.clear();
	axTempString tmpName(name);
	auto* s = ::getenv(tmpName.c_str());
	if (!s) return false;

	outValue.set_c_str(s);
	return true;
#endif
}

};
