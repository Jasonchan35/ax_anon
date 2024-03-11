#include <ax_core/time/axTimestamp.h>

class Test_axHiResTime : public axUnitTestClass {
public:
	void test_case1() {
		axInt foo = 0;

		auto start = axHiResTime::s_now();
		for (axInt i = 0; i < 1000; i++) {
			foo += i;
		}
		auto end = axHiResTime::s_now();
		AX_DUMP_VAR(start);
		AX_DUMP_VAR(end);

		AX_DUMP_VAR(foo);

		auto diff = (end - start);
		AX_DUMP_VAR(diff);
		AX_DUMP_VAR(diff.toMicroseconds());
		AX_DUMP_VAR(diff.toMilliseconds());
		AX_DUMP_VAR(diff.toSeconds());
	}
};

void test_axHiResTime() {
 	AX_TEST_CASE(Test_axHiResTime, test_case1());
}
