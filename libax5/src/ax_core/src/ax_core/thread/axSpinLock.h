#pragma once
#include "axLockProtected.h"
#include "axThreadUtil.h"

//! Internal Spin Lock used by axAllocator or axPerfProfiler
class axInternalSpinLock : public axNonCopyable {
	using This = axInternalSpinLock;
public:
	using ScopedLock = axScopedLock<This>;

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

	AX_INLINE axInternalSpinLock	()	{ _ctor(); }
	AX_INLINE ~axInternalSpinLock	()	{ _dtor(); }

	AX_INLINE bool	tryLock	() { return _tryLock(); }
	AX_INLINE void	lock	() { _lock(); }
	AX_INLINE void unlock	() { _unlock(); }

private:
#if 1 // use std::atomic_flag
	AX_INLINE void	_ctor	()		{}
	AX_INLINE void	_dtor	()		{}
	AX_INLINE bool	_tryLock()		{ return !_v.test_and_set(std::memory_order_acquire); }
	AX_INLINE void	_lock	()		{
	#if AX_CPLUSPLUS_20
		while (_v.test_and_set(std::memory_order_acquire)) {
			_v.wait(true, std::memory_order_relaxed);
		}
	#else
		for (axInt i = 0; _v.test_and_set(std::memory_order_acquire); i++) {
			if (i < 8) continue;
			//else if (i < 16) __asm__ __volatile__("rep; nop" : : : "memory");
			else if(i < 64) axThreadUtil::yield();
			else {
				ax_sleep(1);
			}
		}
	#endif
	}
	AX_INLINE void	_unlock	()		{
		_v.clear(std::memory_order_release);
	#if AX_CPLUSPLUS_20
		_v.notify_one();
	#endif
	}

	std::atomic_flag _v = ATOMIC_FLAG_INIT;

#elif 0 // mutex
	AX_INLINE void	_ctor	()		{}
	AX_INLINE void	_dtor	()		{}
	AX_INLINE bool	_tryLock()		{ return _v.tryLock(); }
	AX_INLINE void	_lock	()		{ _v.lock(); }
	AX_INLINE void	_unlock	()		{ _v.unlock(); }

	::std::mutex _v;


#elif AX_OS_LINUX
	AX_INLINE void	_ctor()			{ pthread_spin_init(&_v, PTHREAD_PROCESS_PRIVATE); }
	AX_INLINE void	_dtor()			{ pthread_spin_destroy(&_v); }
	AX_INLINE bool	_tryLock()		{ return pthread_spin_trylock(&_v); }
	AX_INLINE void	_lock	()		{ pthread_spin_lock(&_v); }
	AX_INLINE void	_unlock	()		{ pthread_spin_unlock(&_v); }

	pthread_spinlock_t _v;

#elif AX_USE_Win32Api
	AX_INLINE void	_ctor	()		{ InitializeCriticalSectionAndSpinCount(&_v, 4000); }
	AX_INLINE void	_dtor	()		{ DeleteCriticalSection(&_v); }
	AX_INLINE bool	_tryLock()		{ if (TryEnterCriticalSection(&_v)) { _checkRecursiveLock(); _locked = true; return true; } return false; }
	AX_INLINE void	_lock	()		{ EnterCriticalSection(&_v); _checkRecursiveLock(); _locked = true; }
	AX_INLINE void	_unlock	()		{ _locked = false; LeaveCriticalSection(&_v); }

	AX_INLINE void	_checkRecursiveLock() { AX_ASSERT(!_locked); }

	CRITICAL_SECTION _v;
	bool	_locked = false;

#elif AX_OS_MACOSX || AX_OS_IOS
	AX_INLINE void	_ctor	()		{ _v = OS_UNFAIR_LOCK_INIT; }
	AX_INLINE void	_dtor	()		{}
	AX_INLINE bool	_tryLock()		{ return os_unfair_lock_trylock(&_v); }
	AX_INLINE void	_lock	()		{ return os_unfair_lock_lock(&_v); }
	AX_INLINE void	_unlock	()		{ return os_unfair_lock_unlock(&_v); }
	
	os_unfair_lock  _v;
		
#else
	#error "axSpinLock implementation is missing"
#endif
};

//! Non Recursive Spin Lock
class axSpinLock : public axNonCopyable {
	typedef axSpinLock This;
public:
	typedef axScopedLock<This> ScopedLock;

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

	AX_INLINE bool	tryLock	() { return _impl.tryLock(); }
	AX_INLINE void	lock	() { _impl.lock(); }
	AX_INLINE void unlock	() { _impl.unlock(); }
private:
	axInternalSpinLock _impl;
};

template<class VALUE> using axSpinLockProtected = axLockProtected<axSpinLock, VALUE>;
template<class VALUE> using axInternalSpinLockProtected = axLockProtected<axInternalSpinLock, VALUE>;