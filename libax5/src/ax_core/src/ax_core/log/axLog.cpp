#include <ax_core/log/axLog.h>

axLog g_axLog;

bool axLog::s_showSourceLocation = false;

axLogIOutput::axLogIOutput() {
}

axLogStdOutput* axLogStdOutput::s_instance() {
	static axLogStdOutput s;
	return &s;
}

void axLogStdOutput::write(Level lv, axStrView s) { 
	ax_console_write(s); 
	#if AX_OS_WINDOWS
		if (IsDebuggerPresent()) {
			axTempStringW ws;
			ws.setUtf(s);
			OutputDebugString(ws.c_str());
		}
	#endif
}

void axLogStdOutput::flush() { std::flush(std::wcout); }

axLogIOutput* axLog::_output = axLogStdOutput::s_instance();

void axLog::logParams(const axSourceLoc& loc, Level lv, axStrView fmt, const axSpan<axFormatParam>& paramList) {
	#if AX_OS_WINDOWS
		SetConsoleOutputCP(CP_UTF8);
	#endif

	auto showSourceLocation = s_showSourceLocation;

	axTempString s;
	switch (lv) {
		case Level::Warning:	s.append("Warning: ");	showSourceLocation = true; break;
		case Level::Error:		s.append("Error: ");	showSourceLocation = true; break;
	}

	s.appendFormatWithParamList(fmt, paramList);
	s.append('\n');

	if (showSourceLocation && loc) {
		s.appendFormat("{?}\n\n", loc);
	}

	if (_output) {
		_output->write(lv, s);
	}
}

void axLog::logHex(const axSourceLoc& loc, Level lv, axStrView msg, axByteSpan span) {
	axTempString s;
	const axInt bytePerLine = 16;
	auto lineCount = ax_roundup_div(span.size(), bytePerLine);
	s.reserve(msg.size() + lineCount * bytePerLine * 5 + 20);
	s.append(msg);

	axInt i = 0;
	for (auto& c : span) {
		if (i % bytePerLine == 0) {
			s.appendFormat("\n{?}: ", static_cast<const void*>(&c));
		}
		s.appendFormat("{?,2:0X} ", c);
		i++;
	}

	log(loc, lv, "{?}", s);
}

void axLog::flush() {
	if (_output) {
		_output->flush();
	}
}

#if AX_OS_WINDOWS

axWin32_ErrorCodeString::axWin32_ErrorCodeString(DWORD errorCode) {
    LPWSTR tmp;
    FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR)&tmp, 0, NULL );

	_str.setUtf_c_str(tmp);
    LocalFree(tmp);
}

axWin32_HRESULT_String::axWin32_HRESULT_String(DWORD hr) {
	_com_error err(hr, nullptr);
	auto* tmp = err.ErrorMessage();
	_str.setUtf_c_str(tmp);
}

#endif //AX_OS_WINDOWS
