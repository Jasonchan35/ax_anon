#pragma once

#include "../container/axDeque.h"
#include "../container/axIndirectArray.h"
#include "axThread.h"
#include "axSpinLock.h"
#include "axCondMutex.h"
#include "axCondInt.h"
#include "../other/axDelegate.h"

class axJobManager;
using axJobFunc = axFunc<void()>;

#define axJobStatus_EnumList(E) \
	E(None,) \
	E(Wait,) \
	E(Running,) \
	E(Done,) \
//------
ax_ENUM_CLASS(axJobStatus, axInt8)

class axJobId {
public:
	void wait();
	bool isDone();

friend class axJobManager;
friend class axJobThread;
protected:
	struct SharedInfo : public axRefCounted {
		axAtomicT<axJobStatus> status;
	};
	axSPtr<SharedInfo>	_info;
};

class axJob : public axDListNode<axJob> {
public:
	virtual ~axJob() = default;
	virtual void onJobRun() {}

friend class axJobManager;
friend class axJobThread;
protected:
	axArray<axJobId, 8>			_jobDepends;
	axJobId						_jobId;
};

class axJobThread : public axThread {
	using Base = axThread;
public:
	void start();
	void onFormat(axFormat& f) const;
	axInt threadIndex() const { return _threadIndex; }

friend class axJobManager;
protected:
	void onThreadProc();
	axJobManager* _mgr = nullptr;
	axInt	_threadIndex = 0;
};

class axJobManager_CreateDesc {
public:
	axStrView name;
	axInt threadCount = 1;

	axInt idleTimeInMilliseconds = 100;

	axFunc<void (axInt threadIndex)> onThreadStart = nullptr;
	axFunc<void (axInt threadIndex)> onWaitTimeout = nullptr;

};

class axJobManager : public axNonCopyable {
public:
	using CreateDesc = axJobManager_CreateDesc;

	void create(CreateDesc& desc);
	void destroy();

	axJobId addJob(const axSourceLoc& loc, axJobFunc func,       axSpan<axJobId> depends = axSpan<axJobId>());
	axJobId addJob(const axSourceLoc& loc, axUPtr<axJob> && job, axSpan<axJobId> depends = axSpan<axJobId>());

	~axJobManager();

	axStrView name() const { return _desc.name; }

	axInt pendingJobCount();

friend class axJobDesc;
friend class axJobThread;
protected:

	struct MData {
		bool stopping = false;
		axDList<axJob>	pendingJobs;
	};
	axCondMutexProtected<MData, false>	_mdata;
	CreateDesc	_desc;

private:
	axIndirectArray<axJobThread, 64>	_cpuThreads;
};


