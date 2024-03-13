#pragma once
#include "axLockProtected.h"

#if AX_CPLUSPLUS_17

#include <shared_mutex>

class axSharedMutex : public axNonCopyable {
public:
	//exclusive lock
	bool	tryLock	()		{ return _v.try_lock(); }
	void	lock	()		{ _v.lock(); }
	void	unlock	()		{ _v.unlock(); }

	bool	tryLockRead	()	{ return _v.try_lock_shared(); }
	void	lockRead	()	{ _v.lock_shared(); }
	void	unlockRead	()	{ _v.unlock_shared(); }
private:
	::std::shared_mutex _v;
};

#else //-------------
class axSharedMutex : public axNonCopyable {
public:
	//exclusive lock
	bool tryLock() {
		if (!_mutex.try_lock())
			return false;

		UniqueLock r(_readMutex);
		if (_readers) {
			_mutex.unlock();
			return false;
		}
		return true;
	}

	void lock() { 
		_mutex.lock();
		UniqueLock r(_readMutex);
		while (_readers) {
			_readCv.wait(r);
		}
	}

	void unlock() {
		_mutex.unlock();
	}

	//----------------
	bool tryLockRead()	{ 
		if (!_mutex.try_lock())
			return false;

		UniqueLock r(_readMutex);
		_readers++;
		_mutex.unlock();
		return true;
	}

	void lockRead()	{ 
		_mutex.lock();
		UniqueLock r(_readMutex);
		_readers++;
		_mutex.unlock();
	}

	void unlockRead()	{
		UniqueLock r(_readMutex);
		if (_readers == 0) {
			AX_ASSERT(false);
			return;
		}

		if (--_readers) {
			_readCv.notify_all();
		}
	}

private:
	using UniqueLock = ::std::unique_lock<::std::mutex>;

	::std::mutex	_mutex;
	::std::condition_variable	_readCv;
	::std::mutex	_readMutex;

	axInt _readers = 0;
};
#endif

template<class VALUE> using axSharedMutexProtected = axSharedLockProtected<axSharedMutex, VALUE>;

