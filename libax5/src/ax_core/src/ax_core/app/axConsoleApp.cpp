#include "axConsoleApp.h"

void axConsoleApp::setConsoleTitle(axStrView title) {
#if AX_OS_WINDOWS
	::SetConsoleTitle(axTempStringW::s_convert(title).c_str());
#else

#endif
}

