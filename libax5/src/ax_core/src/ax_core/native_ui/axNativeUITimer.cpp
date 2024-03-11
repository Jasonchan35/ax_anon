#include "axNativeUIApp.h"
#include "axNativeUITimer.h"
#include "../other/axGlobalSingleton.h"

void axNativeUITimer::start(axObject* obj, Func func, axInt intervalMillisecond, bool repeats) {
	stop();

	_delegate.bind(obj, func);
	_intervalMillisecond = intervalMillisecond;
	_repeats = repeats;
	_start();
}

void axNativeUITimer::stop() {
	_stop();
}

#if AX_OS_WINDOWS

class axUITimerManager : public axNonCopyable {
	using This = axUITimerManager;
public:
	static This* s_instance() {
		static axGlobalSingleton<This> s;
		return s.ptr();
	}

	axDict<UINT_PTR, axNativeUITimer*>	_timerDict;
};

void axNativeUITimer::_start() {
	UINT t;
	ax_safe_assign(t, _intervalMillisecond);
	_p = ::SetTimer(nullptr, 0, t, s_onTimer);
	axUITimerManager::s_instance()->_timerDict.add(_p, this);
}

void axNativeUITimer::_stop() {
	if (_p) {
		axUITimerManager::s_instance()->_timerDict.remove(_p);
		::KillTimer(nullptr, _p);
		_p = 0;
	}
}

void CALLBACK axNativeUITimer::s_onTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	if (!idEvent) {
		AX_ASSERT(false);
		return;
	}

	auto** ppTimer = axUITimerManager::s_instance()->_timerDict.find(idEvent);
	if (!ppTimer) {
		AX_ASSERT(false);
		return;
	}
	
	auto& timer = **ppTimer;

	if (!timer._repeats) {
		timer._stop();
	}
	timer._fire();
}

#elif AX_OS_MACOSX || AX_OS_IOS

@interface axNativeUITimer_Obj : NSObject {
	axNativeUITimer* _owner;
	NSTimer* _timer;
}
@end

@implementation axNativeUITimer_Obj
- (void)onTimer:(NSTimer*)timer {
	_owner->_fire();
}

- (void)stop {
	if (_timer) {
		[_timer invalidate];
		_timer = nil;
	}
}

- (instancetype)init:(axNativeUITimer*)owner interval:(NSTimeInterval)interval repeats:(bool)repeats {
	self = [super init];
	_owner = owner;
	_timer = [NSTimer scheduledTimerWithTimeInterval:interval target:self selector:@selector(onTimer:) userInfo:nil repeats:repeats];
	return self;
}
@end

void axNativeUITimer::_start() {
	auto t = _intervalMillisecond / 1000.0;
	_p.ref([[axNativeUITimer_Obj alloc] init:this interval:t repeats:_repeats]);
}

void axNativeUITimer::_stop() {
	if (_p) {
		[_p stop];
		_p.unref();
	}
}

#elif AX_USE_X11

void axNativeUITimer::_updateNextFireTime() {
	_nextFireTime.setToNow();
	_nextFireTime += Time::s_milliseconds(_intervalMillisecond);
}

void axNativeUITimer::_start() {
	AX_ASSERT(!_addedToUIApp);

	_updateNextFireTime();	
	auto* app = axNativeUIApp_X11::s_instance();
	_addedToUIApp = true;
	app->_timers.append(this);
}

void axNativeUITimer::_stop() {
	if (_addedToUIApp) {
		auto* app = axNativeUIApp_X11::s_instance();
		auto ret = app->_timers.findAndRemove(this);
		if (ret < 0) AX_ASSERT(false);
		_addedToUIApp = false;
	}
}

#elif AX_OS_ANDROID
void axNativeUITimer::_start() {
	AX_ASSERT_NOT_IMPLEMENTED();
}

void axNativeUITimer::_stop() {
	AX_ASSERT_NOT_IMPLEMENTED();
}

#endif

