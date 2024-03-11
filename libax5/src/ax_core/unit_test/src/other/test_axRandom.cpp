#include <ax_core/other/axRandom.h>

class Test_axRandom : public axUnitTestClass {
public:
	void test_int() {
		axRandom rand;
		const axInt64 min = -10;
		const axInt64 max = 10;

		for (axInt i = 0; i < 10000; i++) {
			auto r = rand.range(min, max);
			//AX_DUMP_VAR(r);
			AX_TEST_CHECK_SLIENT(r >= min && r <= max);
		}
	}

	void test_float() {
		axRandom rand;

		const axFloat32 min = -10;
		const axFloat32 max = 10;

		for (axInt i = 0; i < 10000; i++) {
			auto r = rand.range(min, max);
			//AX_DUMP_VAR(r);
			AX_TEST_CHECK_SLIENT(r >= min && r <= max);
		}
	}
};

void test_axRandom() {
 	AX_TEST_CASE(Test_axRandom, test_int());
 	AX_TEST_CASE(Test_axRandom, test_float());
}
