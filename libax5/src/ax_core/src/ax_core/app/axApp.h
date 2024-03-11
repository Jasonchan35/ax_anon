#pragma once

#include "../other/axObject.h"

class axAppError : axError {};
class axAppError_Undefined : axAppError {};

class axAppArgments : public axNonCopyable {
public:
	static axAppArgments* getInstace();

	axAppArgments(int argc, const char* argv[]);
	~axAppArgments();

	axSpan<axStrView>	args() const { return _argsView; }
private:
	axArray<axStrView>	_argsView;
	axArray<axString>	_argsStr;
};

class axApp : public axObject {
	AX_RTTI_CLASS(axApp, axObject)
public:
	static axApp* s_instance();

	axApp();
	virtual ~axApp();

			void	create()	{ onCreate(); }
	virtual void	onCreate() {}
	virtual void	willQuit() {}	
	virtual int		onRun() = 0;

			void	setAppName(axStrView s);
		axStrView	appName();

	axStrView currentExecuteFilename() const;
	axSpan<axStrView> commandArguments() const;

	int	_run();

private:
	axString	_appName;
};

template<class T> inline
int axApp_run(int argc, const char* argv[]) {
	ax_core_init();
	axAppArgments arg(argc, argv);
	
	T app;
	int ret = app._run();
	return ret;
}

#if AX_OS_WINDOWS
	#define axApp_main(T) \
		int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) { return axApp_run<T>(0, nullptr); } \
		int main(int argc, const char* argv[]) { \
			try { \
				return axApp_run<T>(argc, argv); \
			} catch(...) { \
				axLog::flush(); \
				throw; \
			} \
		} \
	//--------
#else
	#define axApp_main(T) \
		int main(int argc, const char* argv[]) { return axApp_run<T>(argc, argv); } \
	//--------
#endif
