#pragma once

#include <ax_core/time/axTimestamp.h>
#include <ax_core/other/axDelegate.h>

#if AX_OS_MACOSX || AX_OS_IOS
@class axNativeUITimer_Obj;
#endif

//! Timer in UI Main Loop
class axNativeUITimer : public axNonCopyable {
public:
	using Time = axNativeUIEventTime;
	using Func = axFunc<void ()>;

	~axNativeUITimer() { stop(); }

	void start(axObject* obj, Func func, axInt intervalMillisecond, bool repeat);
	void stop();

	void _fire() { _delegate.emit(); }

private:
	void _start();
	void _stop();


#if AX_OS_WINDOWS
	static void CALLBACK s_onTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	UINT_PTR _p = 0;
	
#elif AX_OS_MACOSX || AX_OS_IOS
	axNSObj<axNativeUITimer_Obj> _p;

#elif AX_USE_X11
friend class axNativeUIApp_X11;
protected:
	void _updateNextFireTime();
	Time	_nextFireTime;
	bool	_addedToUIApp = false;

private:
#endif

	axDelegate<void ()>	_delegate;
	axInt	_intervalMillisecond = 0;
	bool	_repeats;
};
