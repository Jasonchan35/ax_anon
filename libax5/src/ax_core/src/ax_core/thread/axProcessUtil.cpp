#include "axProcessUtil.h"

#if AX_OS_WINDOWS
/*
void axProcessUtil::getAffinity(axThreadAffinityMask& outMask) {
	DWORD_PTR  mask, sysMask;
	if (0 == ::GetProcessAffinityMask(GetCurrentProcess(), &mask, &sysMask)) {
		throw axError_Undefined(AX_LOC);
	}

	AX_STATIC_ASSERT(sizeof(mask) == sizeof(outMask));
	outMask = mask;
	return;
}

void axProcessUtil::setAffinity(axThreadAffinityMask mask) {
	AX_STATIC_ASSERT(sizeof(mask) == sizeof(DWORD_PTR));
	if (0 == ::SetProcessAffinityMask(GetCurrentProcess(), mask)) {
		throw axError_Undefined(AX_LOC);
	}
}
*/
#else



#endif
