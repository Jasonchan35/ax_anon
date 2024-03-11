#include <ax_core/base/ax_base.h>
#include <ax_core/system/axSystemInfo.h>
#include <ax_core/string/axString.h>

axStrLiteral axSystemInfo::getOSName() {
	return ax_enum_name(getOS());
}

axSystemInfo::OS axSystemInfo::getOS() {
#if AX_OS_WINDOWS
	return OS::Windows;
#elif AX_OS_ANDROID
	return OS::Android;
#elif AX_OS_MACOSX
	return OS::MacOSX;
#elif AX_OS_IOS
	return OS::iOS;
#elif AX_OS_LINUX
	return OS::Linux;
#elif AX_OS_CYGWIN
	return OS::Cygwin;
#else
	#error "Unknown OS"
#endif
}

axInt axSystemInfo::cpuCount() {
	return std::thread::hardware_concurrency();
}
