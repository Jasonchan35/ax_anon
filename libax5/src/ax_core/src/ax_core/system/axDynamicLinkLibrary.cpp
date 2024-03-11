#include "axDynamicLinkLibrary.h"

#if AX_OS_WINDOWS

bool axDynamicLinkLibrary::load(axStrView filename) {
	unload();
	_h = LoadLibrary(axTempStringW::s_convert(filename).c_str());
	return _h;
}

void axDynamicLinkLibrary::unload() {
	if (_h) {
		FreeLibrary(_h);
		_h = nullptr;
	}
}

axDynamicLinkLibrary::FuncAddr axDynamicLinkLibrary::getProcAddress(axStrView name) {
	if (!_h) return nullptr;
	return GetProcAddress(_h, axTempString::s_convert(name).c_str());
}

#else //AX_OS_WINDOWS

#include <dlfcn.h>

bool axDynamicLinkLibrary::load(axStrView filename) {
	unload();
	_h = dlopen(axTempString::s_convert(filename).c_str(), RTLD_NOW);
	return _h;
}

void axDynamicLinkLibrary::unload() {
	if (_h) {
		dlclose(_h);
		_h = nullptr;
	}
}

axDynamicLinkLibrary::FuncAddr axDynamicLinkLibrary::getProcAddress(axStrView name) {
	if (!_h) return nullptr;
	return dlsym(_h, axTempString::s_convert(name).c_str());
}


#endif 
