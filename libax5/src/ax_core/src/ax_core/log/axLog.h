#pragma once
#include "../string/axFormat.h"

//#define AX_LOG(...) g_axLog.log(__VA_ARGS__)

enum class axLogLevel {
	Unknown,
	Info,
	Warning,
	Error,
};

class axLogIOutput {
public:
	using Level = axLogLevel;

	axLogIOutput();
	virtual ~axLogIOutput() {}

	virtual void write(Level lv, axStrView s) = 0;
	virtual void flush() = 0;
};

class axLogStdOutput : public axLogIOutput {
public:
	static axLogStdOutput* s_instance();
	virtual void write(Level lv, axStrView s) override;
	virtual void flush() override;
};

class axLog {
public:
	using Level = axLogLevel;
	static bool s_showSourceLocation;

	static void log(const axSourceLoc& loc, Level lv, axStrView fmt) {
		logParams(loc, lv, fmt, axSpan<axFormatParam>());
	}

	template<class... Args>
	static void log(const axSourceLoc& loc, Level lv, axStrView fmt, const Args&... args) {
		axFormatParam tmp[] = {args...};
		auto list = axSpan_make(tmp);
		logParams(loc, lv, fmt, list);
	}

	static void logParams(const axSourceLoc& loc, Level lv, axStrView fmt, const axSpan<axFormatParam>& paramList);

	static void logHex(const axSourceLoc& loc, Level lv, axStrView msg, axByteSpan span);

	static void flush();

	static void setOutput(axLogIOutput* p) { _output = p; }

private:
	static axLogIOutput* _output;
};

extern axLog g_axLog;

//template<class Format, class ...Args> inline
//void AX_LOG(const Format& fmt, const Args &... args) { g_axLog.log(fmt, args...); }

#define AX_LOG(fmt, ...)		do{ axLog::log(AX_LOC, axLog::Level::Info,	  fmt, ##__VA_ARGS__); }while(false)
#define AX_LOG_WARNING(fmt, ...)	do{ axLog::log(AX_LOC, axLog::Level::Warning, fmt, ##__VA_ARGS__); }while(false)
#define AX_LOG_ERROR(fmt, ...)		do{ axLog::log(AX_LOC, axLog::Level::Error,	  fmt, ##__VA_ARGS__); }while(false)

#define ax_log_func_name()		AX_LOG("FUNC {?}", AX_FUNC_FULLNAME);

#define ax_warning_once(fmt, ...) do{AX_RUN_ONCE( AX_LOG_WARNING(fmt, ##__VA_ARGS__) ); }while(false)
#define ax_flush_log()			do { axLog::flush(); }while(false)

#if AX_OS_WINDOWS

class axWin32_ErrorCodeString {
public:
	axWin32_ErrorCodeString(DWORD errorCode);

	axStrView	strView() const		{ return _str; }
	operator	axStrView() const	{ return strView(); }

	void onFormat(axFormat& f) const { f << strView(); }

private:
	axString_<256>	_str;
};

class axWin32_HRESULT_String {
public:
	axWin32_HRESULT_String(DWORD hr);

	axStrView	strView() const		{ return _str; }
	operator	axStrView() const	{ return strView(); }

	void onFormat(axFormat& f) const { f << strView(); }

private:
	axString_<256>	_str;
};

#define AX_LOG_WIN32_LAST_ERROR(msg)		do{ AX_LOG("Win32 Error {?} {?}" ,axWin32_ErrorCodeString(GetLastError()), msg); }while(false)
#define AX_LOG_WIN32_HRESULT(code, msg)		do{ AX_LOG("HRESULT Error {?,8:X} {?}: {?}", code, axWin32_HRESULT_String(code),  axStrView(msg)); }while(false)

#endif //AX_OS_WINDOWS
