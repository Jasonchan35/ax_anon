#pragma once

#include "../base/ax_base.h"

template<class LOCK>
class axOptionalLock : public axNonCopyable {
	using This = axOptionalLock;
public:
	using LockType = LOCK;
	using ScopedLock = axScopedLock<This>;

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

	AX_INLINE bool	tryLock	()	{ if (!_useLock) return false; return _lock.tryLock(); }
	AX_INLINE void	lock	()	{ if (!_useLock) return; _lock.lock(); }
	AX_INLINE void unlock	()	{ if (!_useLock) return; _lock.unlock(); }

	void setUseLock(bool b) { _useLock = b; }
	bool useLock() const { return _useLock; }

private:
	LockType	_lock;
	bool		_useLock = true;
};