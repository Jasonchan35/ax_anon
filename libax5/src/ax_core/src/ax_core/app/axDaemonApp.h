#pragma once

#include "axConsoleApp.h"

class axDaemonApp : public axConsoleApp {
	AX_RTTI_CLASS(axDaemonApp, axConsoleApp)
public:
	static axDaemonApp* s_instance();

	axDaemonApp();
	virtual ~axDaemonApp();

	bool quitApp() const { return _quitApp; }

protected:
	virtual void onDaemonRun();
	virtual void onCreate() override {}
	virtual void onStart() {}
	virtual void onStop() {}
	virtual void onTick() { ax_sleep(100); }

	void setAppName(axStrView name) { _name = name; }

private:
#if AX_OS_WINDOWS
	static	BOOL	WINAPI s_winConsoleCtrlHandler(DWORD dwCtrlType);
	static	void	WINAPI s_winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors);
	static	DWORD	WINAPI s_winServiceStatusHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	void _winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors);
	void _setWinServiceStatus(DWORD status);

	void installWinService(axStrView params, bool autoStart, axStrView description = axStrView());
	void removeWinService();

	SERVICE_STATUS_HANDLE _winServiceStatusHandle = 0;
#else
	static 	void s_singalHandler(int sig);
			bool _setupDaemon();

#endif

	virtual int onRun() final;
	void _daemonRun();

	axString	_name;
	bool		_quitApp = false;
};
