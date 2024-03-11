#pragma once
#include <ax_core/base/ax_base.h>

/* usage:
	auto ax_ = ax_lock(mutex)
	auto ax_ = ax_lock(mutex0, mutex1)
*/

class axTryLock {};

template<class MUTEX>
class axScopedLock : public axNonCopyable {
public:
	using Mutex = MUTEX;

	axScopedLock() = default;
	axScopedLock(axScopedLock && r) : _mutex(r._mutex) { r._mutex = nullptr; }

	explicit axScopedLock(MUTEX& mutex) { lock(mutex); }
	explicit axScopedLock(const axTryLock&, MUTEX& mutex) { tryLock(mutex); }

	~axScopedLock() { unlock(); }

	explicit operator bool() const { return isLocked(); } 

	bool isLocked() const { return _mutex; }

	void lock	(MUTEX& mutex) { if (_mutex != &mutex) { unlock(); mutex.lock(); _mutex = &mutex; } }
	bool tryLock(MUTEX& mutex) { return _tryLock(mutex); }

	void unlock	()	{ if (_mutex) { _mutex->unlock(); _mutex = nullptr; } }

	void detach	()	{ _mutex = nullptr; }

	MUTEX* mutex() { return _mutex; }
protected:

	bool _tryLock(MUTEX& mutex) {
		unlock();
		if (mutex.tryLock()) {
			_mutex = &mutex;
			_mutex->lock();
			return true;
		}
		return false;
	}

	MUTEX* _mutex = nullptr;
};

template<class MUTEX>
class axScopedReadLock : public axNonCopyable {
public:
	using Mutex = MUTEX;

	axScopedReadLock() = default;
	axScopedReadLock(axScopedReadLock && r) : _v(r._v) { r._v = nullptr; }
	explicit axScopedReadLock(MUTEX& p) { lock(p); }

	~axScopedReadLock() { unlock(); }

	void lock(MUTEX& p) { unlock(); _v = &p; _v->lockRead(); }
	void unlock()		{ if (_v) { _v->unlockRead(); _v = nullptr; } }

	void detach() { _v = nullptr; }

	MUTEX* mutex() { return _v; }
private:
	MUTEX* _v = nullptr;
};

template<class T0, class T1>
class axScopedLock2 : public axNonCopyable {
public:
	axScopedLock2(axScopedLock2 && r) : m0(ax_move(r.m0)), m1(ax_move(r.m1)) {}

	explicit axScopedLock2(T0& t0, T1& t1) {
		if (&t0 < &t1) { //using address to decide the order
			m0.lock(t0);
			m1.lock(t1);
		}else{
			m1.lock(t1);
			m0.lock(t0);
		}
	}

	explicit axScopedLock2(const axTryLock&, T0& t0, T1& t1) {
		if (&t0 < &t1) { //using address to decide the order
			m0.tryLock(t0);
			m1.tryLock(t1);
		}else{
			m1.tryLock(t1);
			m0.tryLock(t0);
		}
	}

	explicit operator bool() const { return isLocked(); }

	bool isLocked() const { return m0.isLocked() && m1.isLocked(); }

	axScopedLock<T0> m0;
	axScopedLock<T1> m1;
};

template<class T0>           inline axScopedLock< T0>     axScopedLock_lock(T0& t0)         { return axScopedLock< T0    >(t0); }
template<class T0, class T1> inline axScopedLock2<T0, T1> axScopedLock_lock(T0& t0, T1& t1) { return axScopedLock2<T0, T1>(t0, t1); }

template<class T0>           inline axScopedLock< T0>     axScopedLock_tryLock(T0& t0)         { return axScopedLock< T0    >(axTryLock(), t0); }
template<class T0, class T1> inline axScopedLock2<T0, T1> axScopedLock_tryLock(T0& t0, T1& t1) { return axScopedLock2<T0, T1>(axTryLock(), t0, t1); }

#define ax_LOCK(T0)		auto AX_UNIQUE_NAME(ax_LOCK_scope) = axScopedLock_lock(T0);
#define ax_TRY_LOCK(T0)	auto AX_UNIQUE_NAME(ax_LOCK_scope) = axScopedTryLock_lock(T0);