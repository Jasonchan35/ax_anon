#include <ax_core/time/axDateTime.h>

class Test_axDateTime : public axUnitTestClass {
public:
	void test_case1() {
		axDateTime dt;
		dt.setToNow();
		AX_DUMP_VAR(dt, dt.weekday());

		dt.addDays(1);
		dt.addHours(1);
		AX_DUMP_VAR(dt, dt.weekday());
	}
};

void test_axDateTime() {
 	AX_TEST_CASE(Test_axDateTime, test_case1());
}
