#pragma once

#include "axCondVar.h"

template<bool bBroadcast>
class axCondMutex {
	using This = axCondMutex;
public:
	class ScopedLock {
	public:
		ScopedLock() = default;
		ScopedLock(axCondMutex& condMutex);
		ScopedLock(ScopedLock && r);
		~ScopedLock();

		void lock	(axCondMutex& condMutex);
		bool tryLock(axCondMutex& condMutex);
		void unlock	();
		void detach	();

		void signal	()	{ _condMutex->signal(); }
		void wait	()	{ _condMutex->wait(*this);   }
		bool timedWait(axInt milliseconds) { return _condMutex->timedWait(*this, milliseconds); }

				axCondMutex* condMutex() { return _condMutex; }
		axMutex::ScopedLock& mutexLock() { return _mutexLock; }
	private:
		axMutex::ScopedLock	_mutexLock;
		axCondMutex* _condMutex = nullptr;
	};

	void lock		()	{ _mutex.lock(); }
	bool tryLock	()	{ return _mutex.tryLock(); }

	void unlock		() { 
		_mutex.unlock();
		if (bBroadcast) {
			_cond.broadcast();
		} else {
			_cond.signal();
		}
	}

	void wait		(ScopedLock& locked)		{ _cond.wait(locked.mutexLock()); }
	bool timedWait	(ScopedLock& locked, axInt milliseconds) { 
		return _cond.timedWait(locked.mutexLock(), milliseconds);
	}

	void signal		()							{ _cond.signal(); }
	void broadcast	()							{ _cond.broadcast(); }

	axMutex&	mutex	() { return _mutex; }
	axCondVar&	cond	() { return _cond;  }

private:
	axMutex		_mutex;
	axCondVar	_cond;
};

template<class VALUE, bool bBroadcast>
class axCondMutexProtected : public axLockProtected<axCondMutex<bBroadcast>, VALUE> {
	using Base = axLockProtected<axCondMutex<bBroadcast>, VALUE>;
public:
	template<class... Args>
	axCondMutexProtected(Args&&... args) : Base(ax_forward(args)...) {}

	void signal		() { Base::mutex().signal(); }
	void broadcast	() { Base::mutex().broadcast(); }
};

template<bool bBroadcast>
axCondMutex<bBroadcast>::ScopedLock::ScopedLock(ScopedLock && r)
	: _mutexLock(ax_move(r._mutexLock))
	, _condMutex(r._condMutex)
{
	r._condMutex = nullptr;
}

template<bool bBroadcast>
axCondMutex<bBroadcast>::ScopedLock::ScopedLock(axCondMutex& condMutex)
	: _mutexLock(condMutex.mutex())
{
	_condMutex = &condMutex;
}

template<bool bBroadcast>
axCondMutex<bBroadcast>::ScopedLock::~ScopedLock() {
	unlock();
}

template<bool bBroadcast>
void axCondMutex<bBroadcast>::ScopedLock::lock(axCondMutex& condMutex) {
	unlock();
	_mutexLock.lock(condMutex.mutex());
	_condMutex = &condMutex;
}

template<bool bBroadcast>
bool axCondMutex<bBroadcast>::ScopedLock::tryLock(axCondMutex& condMutex) {
	unlock();
	if (_mutexLock.tryLock(condMutex.mutex())) {
		_condMutex = &condMutex;
		return true;
	}
	return false;
}

template<bool bBroadcast>
void axCondMutex<bBroadcast>::ScopedLock::unlock() {
	if (_condMutex) {
		_condMutex->unlock();
		_condMutex = nullptr;
	}
}

template<bool bBroadcast>
void axCondMutex<bBroadcast>::ScopedLock::detach() {
	_mutexLock.detach();
	_condMutex = nullptr;
}
