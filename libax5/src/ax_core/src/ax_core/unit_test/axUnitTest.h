#pragma once

#include "../other/axObject.h"

inline void ax_unit_test_log(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "libax", msg);
#else
	std::cout << msg;
#endif
}

inline
bool AX_TEST_CHECK_func(const axSourceLoc& loc, bool verbose, bool success, const char* msg) {
	if (success && !verbose)
		return success;

	const int kBufSize = 4096;
	char str[kBufSize + 1];

	snprintf(str, kBufSize, "%s %s\n", (success ? "[ OK ]" : "[FAIL]"), msg);
	ax_unit_test_log(str);
	if (!success) {
		_ax_assert_impl("", msg, loc.func, loc.file, loc.line, str);
	}
	return success;
}

#define AX_TEST_CHECK(a)		AX_TEST_CHECK_func(AX_LOC, true,  bool(a), #a)
#define AX_TEST_CHECK_SLIENT(a)	AX_TEST_CHECK_func(AX_LOC, false, bool(a), #a)

#define	AX_TEST_CASE(TestClass, TestFunc) \
	do{ \
		ax_unit_test_log("\n[--TEST_CASE--] " #TestClass "." #TestFunc "\n"); \
		TestClass o; \
		o.TestFunc; \
	}while(false)
//----


class axUnitTestClass : public axObject {
public:
};
