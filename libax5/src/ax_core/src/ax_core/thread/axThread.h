#pragma once

#include "axThreadUtil.h"

#define axThreadPriority_EnumList(E) \
	E(Unknown,) \
	E(Highest,) \
	E(High,) \
	E(Normal,) \
	E(Low,) \
	E(Lowest,) \
	E(Idle,) \
//-------
ax_ENUM_CLASS(axThreadPriority, axUInt8)

#if AX_OS_MACOSX || AX_OS_IOS
@class axThread_NSThread;
#endif

class axThread : public axNonCopyable {
public:
	using Priority = axThreadPriority;
	using AffinityMask = axThreadAffinityMask;
	using Func = axFunc<void ()>;

	axThread();
	virtual ~axThread();

	void start	(axStrView name, Func func, axInt stackSize = 0);
	void join	();
	void detach	();
	void setPriority(Priority p);
	void setAffinity(const axThreadAffinityMask& mask);

	void onFormat(axFormat& f) const;

	axThreadId	threadId() const { return _threadId; }

private:
	void _ctor();
	void _setPriority(Priority p);
	void _setAffinity(const AffinityMask& mask);
	void _start(axInt stackSize);
	void _invokeFunc();

	axString	_name;
	axThreadId	_threadId;

#if AX_OS_WINDOWS
	static DWORD WINAPI s_proc(LPVOID p);
	
#else
	static void* s_proc(void* p);

#endif
	Priority _priority = Priority::Normal;
	axThreadAffinityMask _affinity;
	Func _func;
};
