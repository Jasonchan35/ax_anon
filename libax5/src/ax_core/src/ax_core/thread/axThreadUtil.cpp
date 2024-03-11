#include "axThreadUtil.h"
#include "../string/axFormatParam.h"
#include "../container/axBitArray.h"

bool axThreadId::isCurrentThread() {
	return *this == s_current();
}

#if AX_OS_WINDOWS

void axThreadId::onFormat(axFormat& f) const {
	f << _v.id;
}

axThreadId axThreadId::s_current() {
	axThreadId o;
	auto h =  ::GetCurrentThread();
	o._v = axThreadId_Native(h, GetThreadId(h));
	return o;
}

#elif AX_OS_MACOSX || AX_OS_IOS

void axThreadId::onFormat(axFormat& f) const {
	f << static_cast<const void*>(_v);
}

axThreadId axThreadId::s_current() {
	axThreadId o;
	o._v = pthread_self();
	return o;
}

#else

void axThreadId::onFormat(axFormat& f) const {
	f << axUInt64(_v);
}

axThreadId axThreadId::s_current() {
	axThreadId o;
	o._v = pthread_self();
	return o;
}

#endif
