#include "axJobManager.h"
#include "../other/axGlobalSingleton.h"
#include "axProcessUtil.h"
#include <ax_core/system/axSystemInfo.h>

axJobManager::~axJobManager() {
	destroy();
}

void axJobManager::create(CreateDesc& desc) {
	_desc = desc;

//	axInt n = ax_max(axInt(1), axSystemInfo::cpuCount() - 1);
//	n = 1; // using 1 thread only for debug

	for (axInt i = 0; i < desc.threadCount; i++) {
		auto* t = _cpuThreads.emplaceBack();
		t->_mgr = this;
		t->_threadIndex = i;
	}

	for(auto& t : _cpuThreads) {
		t->start();
	}
}

void axJobManager::destroy() {
	{
		auto md = _mdata.scopedLock();
		md->stopping = true;
	}

//	AX_LOG("axJobManager stop wait");
	for (auto& t : _cpuThreads) {
		t->join();
	}
//	AX_LOG("axJobManager stopped");
}

axJobId axJobManager::addJob(const axSourceLoc& loc, axJobFunc func, axSpan<axJobId> depends) {
	if (!func) return axJobId();

	struct Job : public axJob {
		virtual void onJobRun() override { func(); }
		axJobFunc func;
	};

	auto job = axUPtr_new(Job());
	job->func = func;
	return addJob(loc, ax_move(job), depends);
}

axJobId axJobManager::addJob(const axSourceLoc& loc, axUPtr<axJob> && job, axSpan<axJobId> depends) {
	if (!job) return axJobId();

	job->_jobDepends = depends;
	auto& jobId = job->_jobId;

	jobId._info.ref(ax_new axJobId::SharedInfo());
	jobId._info->status = axJobStatus::Wait;
	{
		auto md = _mdata.scopedLock();
		md->pendingJobs.append(ax_move(job));
	}

	return jobId;;
}

axInt axJobManager::pendingJobCount() {
	auto md = _mdata.scopedLock();
	return md->pendingJobs.size();
}

void axJobThread::onThreadProc() {
//	AX_LOG("{?} start", this);

	if (_mgr->_desc.onThreadStart) {
		_mgr->_desc.onThreadStart(_threadIndex);
	}

	auto md = _mgr->_mdata.scopedLock();
	for (;;) {
		if (md->stopping) {
			//AX_LOG("{?} stopped", this);
			break;
		}

		auto job = md->pendingJobs.popHead();
		if (!job) {
			//AX_LOG("{?} wait", this);
			if (!md.timedWait(_mgr->_desc.idleTimeInMilliseconds)) {
				if (_mgr->_desc.onWaitTimeout) {
					_mgr->_desc.onWaitTimeout(_threadIndex);
				}
			}
			continue;
		}

		auto& info = job->_jobId._info;
		if (info) {
			info->status = axJobStatus::Running;
		}

		try {
			job->onJobRun();
		} catch (axError& err) {
			AX_LOG_ERROR("Job exception: {?}", err);
		}

		if (info) {
			info->status = axJobStatus::Done;
		}
	}
}

void axJobThread::start() {
	auto name = axTempString::s_format("{?}{?}", _mgr->name(), _threadIndex);
	Base::start(name, [this]() { onThreadProc(); });
}

void axJobThread::onFormat(axFormat& f) const {
	f.format("JobThread[{?}]", threadId());
}

bool axJobId::isDone() {
	if (!_info) return true;
	if (_info->status.get() != axJobStatus::Done) return false;
	return true;
}
