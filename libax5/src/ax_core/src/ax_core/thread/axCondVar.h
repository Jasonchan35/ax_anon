#pragma once
#include "axMutex.h"

class axCondVar : public axNonCopyable {
	using This = axCondVar;
public:
	using Locked = axMutex::ScopedLock;

	axCondVar();
	~axCondVar();

	void	broadcast	();
	void	signal		();
	
	void	wait		(Locked& locked);
	bool	timedWait	(Locked& locked, axInt waitMilliseconds);

private:
#if AX_OS_WINDOWS
	bool	_timedWait	(Locked& locked, DWORD waitMilliseconds);

	union {
		struct { HANDLE _signalEvent, _broadcastEvent; };
		HANDLE _events[2];
	};

	struct MData {
		axInt waitCount = 0;
		axInt broadcastCount = 0;
	};
	axInternalSpinLockProtected<MData> _mdata;

#else
	pthread_cond_t _c;

#endif

//  doesn't support std::recursive_mutex
//	std::condition_variable _v;
};
