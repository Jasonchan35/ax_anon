#include "axThread.h"

axThread::axThread() {
	_ctor();
}

axThread::~axThread() {
	join();
}

void axThread::start(axStrView name, Func func, axInt stackSize) {
	_name = name;

	if (stackSize < 0) throw axError_Undefined(AX_LOC);
	_func = func;
	_start(stackSize);
	if (_priority != Priority::Normal)
		_setPriority(_priority);

	bool needSetAffinity = false;
#if AX_OS_MACOSX || AX_OS_IOS || AX_OS_WINDOWS
	if (_affinity)
		needSetAffinity = true;
#else
	if (CPU_COUNT(&_affinity))
		needSetAffinity = true;
#endif

	if (needSetAffinity)
	_setAffinity(_affinity);
}

void axThread::setPriority(Priority p) {
	_priority = p;
	_setPriority(p);
}

void axThread::setAffinity(const AffinityMask& mask) {
	_affinity = mask;
}

void axThread::onFormat(axFormat& f) const {
	f.format("axThread[{?}]", threadId());
}

void axThread::_invokeFunc() {
	if (!_func) return;
	try {
		_func();
	} catch (axError& err) {
		AX_LOG_ERROR("axThread - {?}", err);
		throw;
	}
}

#if 0
#pragma mark ================= Window ==================
#endif
#if AX_OS_WINDOWS

void axThread::_ctor() {
	_affinity = 0;
}

void axThread::_start(axInt stackSize) {
	if (_threadId) throw axError_Undefined(AX_LOC);
	DWORD flags = CREATE_SUSPENDED;
	auto* h = ::CreateThread(nullptr,
							 static_cast<SIZE_T>(stackSize), 
							 reinterpret_cast<LPTHREAD_START_ROUTINE>(&axThread::s_proc),
							 this, flags, nullptr);
	if (!h) throw axError_Undefined(AX_LOC);
	_threadId._v = axThreadId_Native(h, ::GetThreadId(h));
	::SetThreadDescription(h, axTempStringW::s_convert(_name).c_str());
	ResumeThread(h);
}

DWORD WINAPI axThread::s_proc(LPVOID p) {
	static_cast<axThread*>(p)->_invokeFunc();
	return 0;
}

void axThread::join() {
	if (!_threadId) return;
	WaitForSingleObject(_threadId.nativeHandle(), INFINITE);
	detach();
}

void axThread::detach() {
	if (!_threadId) return;
	CloseHandle(_threadId.nativeHandle());
	_threadId.reset();
}

void axThread::_setPriority(Priority p) {
	if (!_threadId) return;

	int priority = THREAD_PRIORITY_NORMAL;
	switch (p) {
		case Priority::Highest:	priority = THREAD_PRIORITY_HIGHEST;			break;
		case Priority::High:	priority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
		case Priority::Normal:	priority = THREAD_PRIORITY_NORMAL;			break;
		case Priority::Low:		priority = THREAD_PRIORITY_BELOW_NORMAL;	break;
		case Priority::Lowest:	priority = THREAD_PRIORITY_LOWEST;			break;
		case Priority::Idle:	priority = THREAD_PRIORITY_IDLE;			break;
		default: AX_ASSERT(false);
	}
	::SetThreadPriority(_threadId.nativeHandle(), priority);
}

void axThread::_setAffinity(const AffinityMask& mask) {
	if (!_threadId) return;

	::SetThreadAffinityMask(_threadId.nativeHandle(), mask);
}

#if 0
#pragma mark ================= pthread ==================
#endif
#else

void* axThread::s_proc(void* p) {
	static_cast<axThread*>(p)->_invokeFunc();
	return 0;
}

void axThread::_start(axInt stackSize) {
	if (_threadId) throw axError_Undefined(AX_LOC);
	
	pthread_attr_t* pAttr = nullptr;
	pthread_attr_t attr;

	if (stackSize > 0) {
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, static_cast<size_t>(stackSize));
		pAttr = &attr;
	}

	int ret = pthread_create(&_threadId._v, pAttr, &axThread::s_proc, this);

	if (pAttr) {
		pthread_attr_destroy(&attr);
	}

	if (ret != 0) {
		throw axError_Undefined(AX_LOC);
	}
}

void axThread::join() {
	if (!_threadId) return;
	pthread_join(_threadId._v, nullptr);
	detach();	
}

void axThread::detach() {
	if (!_threadId) return;
	pthread_detach(_threadId._v);
	_threadId._v = 0;
}

void axThread::_setPriority(Priority p_) {
	if (!_threadId) return;

	int policy = SCHED_OTHER;
	int priority = 0; // Priority range from -20 (high) ~ 19 (low)
	switch (p_) {
		case Priority::Highest:	policy = SCHED_FIFO;  priority =   0;	break;
		case Priority::High:	policy = SCHED_OTHER; priority = -16;	break;
		case Priority::Normal:	policy = SCHED_OTHER; priority =   0;	break;
		case Priority::Low:		policy = SCHED_OTHER; priority =   8;	break;
		case Priority::Lowest:	policy = SCHED_OTHER; priority =  16;	break;
		case Priority::Idle:	policy = SCHED_OTHER; priority =  19;	break;
		default: AX_ASSERT(false);
	}

	sched_param param;
	param.sched_priority = priority;
	
	pthread_setschedparam(_threadId._v, policy, &param);
}

#if AX_OS_MACOSX || AX_OS_IOS

void axThread::_ctor() {
	_affinity = 0;
}

void axThread::_setAffinity(const AffinityMask& mask) {
	if (!_threadId) return;
	
	integer_t value = mask;
	thread_port_t t = pthread_mach_thread_np(_threadId._v);
	thread_policy_set(t, THREAD_AFFINITY_POLICY, &value, 1);
}

#else

void axThread::_ctor() {
	CPU_ZERO(&_affinity);
}

void axThread::_setAffinity(const AffinityMask& mask) {
	if (!_threadId) return;
	
	cpu_set_t t;
	AX_STATIC_ASSERT(sizeof(t) == sizeof(mask));
	
	t = mask;
#if! AX_OS_ANDROID
	pthread_setaffinity_np(_threadId._v, sizeof(t), &t);
#endif
}

#endif // Mac

#endif



