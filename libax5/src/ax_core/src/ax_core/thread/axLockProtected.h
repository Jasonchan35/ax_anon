#pragma once
#include "axScopedLock.h"

template<class MUTEX, class VALUE>
class axLockProtected : public axNonCopyable {
	using This = axLockProtected;
public:
	using Mutex = MUTEX;
	using Value = VALUE;

	class ScopedLock : public Mutex::ScopedLock {
		using Base = typename Mutex::ScopedLock;
	public:
		ScopedLock(Mutex& mutex, Value& value) : Base(mutex) {
			_value = &value;
		}

		ScopedLock(ScopedLock && r) : Base(ax_move(r)) {
			_value = r._value;
			r._value = nullptr;
		}

		void unlock() {
			Base::unlock();
			_value = nullptr;
		}

		operator Value*	() { return _value; }
		Value* operator->() { return _value; }
		Value*	value() { return _value; }

		Mutex* mutex() { return Base::mutex(); }
	private:
		Value* _value = nullptr;
	};

	template<class... Args>
	axLockProtected(Args&&... args) : _value(ax_forward(args)...) {}

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(_mutex, _value); }

	Mutex& mutex() { return _mutex; }

protected:
	Mutex	_mutex;
	Value	_value;
};

template<class MUTEX, class VALUE>
class axSharedLockProtected : public axNonCopyable {
	typedef axSharedLockProtected This;
public:
	typedef MUTEX Mutex;
	typedef VALUE Value;

	class ScopedLock : public axNonCopyable {
	public:
		ScopedLock(Mutex& mutex, Value& value) {
			_mutex = &mutex;
			_value = &value;
			mutex.lock();
		}

		ScopedLock(ScopedLock && r) : _mutex(r._mutex), _value(r._value) { 
			r._mutex = nullptr;
			r._value = nullptr; 
		}

		~ScopedLock() { unlock(); }

		void unlock() {
			if (_mutex) {
				_mutex->unlock();
				_mutex = nullptr;
				_value = nullptr;
			}
		}

		Value* operator->() { return _value; }
		Mutex* mutex() { return _mutex; }
	private:
		Mutex* _mutex = nullptr;
		Value* _value = nullptr;
	};

	class ScopedReadLock : public axNonCopyable {
	public:
		ScopedReadLock(Mutex& mutex, Value& value) {
			_mutex = &mutex;
			_value = &value;
			mutex.lockRead();
		}

		ScopedReadLock(ScopedLock && r) : _mutex(r._mutex), _value(r._value) { 
			r._mutex = nullptr;
			r._value = nullptr; 
		}

		~ScopedReadLock() { unlock(); }

		void unlock() {
			if (_mutex) {
				_mutex->unlockRead();
				_mutex = nullptr;
				_value = nullptr;
			}
		}

		const Value* operator->() { return _value; }
		Mutex* mutex() { return _mutex; }
	private:
		Mutex* _mutex = nullptr;
		const Value* _value = nullptr;
	};

	template<class... Args>
	axSharedLockProtected(Args&&... args) : _value(ax_forward(args)...) {}

	AX_NODISCARD ScopedLock		scopedLock()		{ return ScopedLock(    _mutex, _value); }
	AX_NODISCARD ScopedReadLock	scopedReadLock()	{ return ScopedReadLock(_mutex, _value); }

protected:
	Mutex	_mutex;
	Value	_value;
};
