#pragma once

#include "../string/axString.h"
#include "../math/axRect2.h"

#define axSystemInfo_OS_EnumList(E) \
	E(Unknown,) \
	E(Windows,) \
	E(Android,) \
	E(Linux,) \
	E(MacOSX,) \
	E(iOS,) \
	E(Cygwin,) \
//---------
ax_ENUM_CLASS(axSystemInfo_OS, axInt32)

class axSystemInfo : public axStaticClass {
public:
	using OS = axSystemInfo_OS;

	static	OS				getOS();
	static	axStrLiteral	getOSName();
	static	axInt			cpuCount();
};

