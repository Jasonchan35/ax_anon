#pragma once
#include "axScopedLock.h"
#include "axLockProtected.h"

//! Internal Mutex used by axAllocator or axPerfProfiler
class axInternalMutex : public axNonCopyable {
	using This = axInternalMutex;
public:
	using ScopedLock = axScopedLock<This>;

	AX_INLINE axInternalMutex()	{ _ctor(); }
	AX_INLINE ~axInternalMutex()	{ _dtor(); }

	AX_INLINE void	lock	() { _lock(); }
	AX_INLINE bool	tryLock	() { return _tryLock(); }
	AX_INLINE void	unlock	() { _unlock(); }

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

private:
// it turns out, cannot use c++11 recursive_mutex, because std::condition_variable only support std::mutex
//	::std::recursive_mutex _v;

#if AX_OS_WINDOWS
public:
	using NativeMutex = CRITICAL_SECTION;

	AX_INLINE	void _ctor()	{ InitializeCriticalSection(&_m); }
	AX_INLINE	void _dtor()	{ DeleteCriticalSection(&_m); }
	AX_INLINE	void _lock()	{ EnterCriticalSection(&_m); }
	AX_INLINE	bool _tryLock()	{ return TryEnterCriticalSection(&_m) != 0; }
	AX_INLINE	void _unlock()	{ LeaveCriticalSection(&_m); }

	NativeMutex* nativeMutex() { return &_m; }
private:
	NativeMutex _m;

#else

public:
	using NativeMutex = pthread_mutex_t;

	AX_INLINE
	void _ctor() {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&_m, &attr);
		pthread_mutexattr_destroy(&attr);
	}

	AX_INLINE void _dtor	() { pthread_mutex_destroy(&_m); }
	AX_INLINE void _lock	() { pthread_mutex_lock(&_m); }
	AX_INLINE bool _tryLock() { return pthread_mutex_trylock(&_m) == 0; }
	AX_INLINE void _unlock	() { pthread_mutex_unlock(&_m); }

	NativeMutex* nativeMutex() { return &_m; }
private:
	NativeMutex _m;
#endif

};

class axMutex : public axNonCopyable {
	using This = axMutex;
public:
	using ScopedLock = axScopedLock<This>;
	using NativeMutex = axInternalMutex::NativeMutex;

	AX_INLINE void	lock	() {
		_impl.lock();
	}

	AX_INLINE bool	tryLock	() {
		return _impl.tryLock();
	}

	AX_INLINE void	unlock	() { _impl.unlock(); }

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

	NativeMutex* nativeMutex() { return _impl.nativeMutex(); }

private:
	axInternalMutex _impl;
};

template<class VALUE> using axInternalMutexProtected = axLockProtected<axInternalMutex, VALUE>;
template<class VALUE> using axMutexProtected         = axLockProtected<axMutex, VALUE>;
