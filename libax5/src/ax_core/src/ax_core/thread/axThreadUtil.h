#pragma once

#include <ax_core/base/ax_base.h>

#if AX_OS_WINDOWS

using axThreadAffinityMask = DWORD_PTR;

struct axThreadId_Native {
	using This = axThreadId_Native;

	axThreadId_Native() = default;
	axThreadId_Native(HANDLE handle_, DWORD id_) : handle(handle_), id(id_) {}

	operator HANDLE() const { return handle; }

	HANDLE	handle = nullptr;
	DWORD	id = 0;
};

constexpr axThreadId_Native axThreadId_Native_kNull = axThreadId_Native();

#elif AX_OS_MACOSX || AX_OS_IOS

using axThreadAffinityMask = integer_t;
using axThreadId_Native = pthread_t;
const axThreadId_Native axThreadId_Native_kNull = nullptr;

#else

using axThreadAffinityMask = cpu_set_t;
using axThreadId_Native = pthread_t;
const axThreadId_Native axThreadId_Native_kNull = 0;

#endif

class axThreadId {
public:
	using NativeHandle = axThreadId_Native;

	void reset() { _v = axThreadId_Native_kNull; }

	static axThreadId s_current();

	void onFormat(axFormat& f) const;
	explicit operator bool() const { return _v != axThreadId_Native_kNull; }
	bool operator==(const axThreadId& r) const { return _v == r._v; }
	bool operator!=(const axThreadId& r) const { return _v != r._v; }

	void setToCurrentThread() { *this = s_current(); }
	bool isCurrentThread();

	NativeHandle nativeHandle() { return _v; }

friend class axThread;
protected:
	axThreadId_Native _v = axThreadId_Native_kNull;
};

class axThreadUtil : public axStaticClass {
public:
#if 1 // C++11
	static AX_INLINE void yield() { ::std::this_thread::yield(); }

#elif AX_OS_WINDOWS
	static AX_INLINE void yield() { ::YieldProcessor(); }

#elif AX_OS_LINUX || AX_OS_IOS || AX_OS_MACOSX
	static AX_INLINE void yield() { ::cpu_relax(); }

#elif AX_COMPILER_GCC
	static AX_INLINE void yield() { __asm__("pause"); }

#elif 1// pthread
	static AX_INLINE void yield() { ::pthread_yield(void); }
#else
	#error
#endif
};
