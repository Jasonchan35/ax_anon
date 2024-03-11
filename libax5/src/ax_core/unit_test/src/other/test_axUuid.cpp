#include <ax_core/other/axUuid.h>

class Test_axUuid : public axUnitTestClass {
public:
	void test_case1() {
		auto a = axUuid::s_generate();
		AX_DUMP_VAR(a);

		auto str = axTempString::s_convert(a);

		axUuid b;
		b.parse(str);

		AX_TEST_CHECK(a == b);
	}
};

void test_axUuid() {
 	AX_TEST_CASE(Test_axUuid, test_case1());
}
