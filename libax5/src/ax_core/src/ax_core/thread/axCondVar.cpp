#include "axCondVar.h"

#if AX_OS_WINDOWS

axCondVar::axCondVar() {
	AX_ASSERT(reinterpret_cast<void*>(&_signalEvent)    == reinterpret_cast<void*>(&_events[0]));
	AX_ASSERT(reinterpret_cast<void*>(&_broadcastEvent) == reinterpret_cast<void*>(&_events[1]));

	_signalEvent = CreateEvent(nullptr, false, false, nullptr);
	if (!_signalEvent) throw axError_Undefined(AX_LOC);

	_broadcastEvent = CreateEvent(nullptr, true, false, nullptr);
	if (!_broadcastEvent) throw axError_Undefined(AX_LOC);
}

axCondVar::~axCondVar() {
	if (_signalEvent)	 CloseHandle(_signalEvent);
	if (_broadcastEvent) CloseHandle(_broadcastEvent);
}

void axCondVar::signal() {
	SetEvent(_signalEvent);
}

void axCondVar::broadcast() {
	auto data = _mdata.scopedLock();

	if (data->waitCount == 0) return;
	data->broadcastCount = data->waitCount;

	SetEvent(_broadcastEvent);
}

void axCondVar::wait(Locked& locked) {
	_timedWait(locked, INFINITE);
}

bool axCondVar::timedWait(Locked& locked, axInt waitMilliseconds) {
	if (waitMilliseconds < 0) {
		AX_ASSERT(false);
		waitMilliseconds = 0;
	}
	return _timedWait(locked, static_cast<DWORD>(waitMilliseconds));
}

bool axCondVar::_timedWait(Locked& locked, DWORD waitMilliseconds) {
	{
		auto data = _mdata.scopedLock();
		data->waitCount++;
	}

	auto* mutex = locked.mutex();
	mutex->unlock();
	DWORD ret = WaitForMultipleObjects(2, _events, false, waitMilliseconds);
	mutex->lock();

	{
		auto data = _mdata.scopedLock();
		data->waitCount--;

		switch(ret) {
			case WAIT_TIMEOUT:
				return false;

			case WAIT_OBJECT_0: // _signalEvent
				return true;

			case WAIT_OBJECT_0+1: { // _broadcastEvent
				data->broadcastCount--;

				if (data->broadcastCount <= 0) {
					// I'm the last one 
					ResetEvent(_broadcastEvent);
				}

				return true;
			}break;

			case WAIT_ABANDONED:
			case WAIT_ABANDONED+1: {
				AX_ASSERT(false);
				return false;
			}break;
		}
	}

	AX_ASSERT(false);
	return false;
}

#else 

axCondVar::axCondVar() {
	pthread_cond_init(&_c, nullptr);
}

axCondVar::~axCondVar() {
	pthread_cond_destroy(&_c);
}

void axCondVar::signal() {
	pthread_cond_signal(&_c);
}

void axCondVar::broadcast() {
	pthread_cond_broadcast(&_c);
}

void axCondVar::wait(Locked& locked) {
	auto* m = locked.mutex();
	pthread_cond_wait(&_c, m->nativeMutex());
}

bool axCondVar::timedWait(Locked& locked, axInt waitMilliseconds) {
	auto* m = locked.mutex();

	timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) < 0) {
		throw axError_Undefined(AX_LOC);
	}

	ts.tv_sec  += waitMilliseconds / 1000;
	ts.tv_nsec += waitMilliseconds % 1000;
		
	int ret = pthread_cond_timedwait(&_c, m->nativeMutex(), &ts);
	switch (ret) {
		case 0:			return true;
		case ETIMEDOUT: return false;
		case EINVAL: throw axError_Undefined(AX_LOC);
	}
	return false;
}

#endif
