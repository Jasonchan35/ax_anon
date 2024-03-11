#include <ax_core/math/axUNorm.h>

class Test_axUNorm : public axUnitTestClass {
public:
	void test_case1() {
		AX_TEST_CHECK(axUNorm8(0).toFloat()       == 0.0f);
		AX_TEST_CHECK(axUNorm16(0).toFloat()      == 0.0f);

		AX_TEST_CHECK(axUNorm8(0xff).toFloat()    == 1.0f);
		AX_TEST_CHECK(axUNorm16(0xffff).toFloat() == 1.0f);

		AX_TEST_CHECK(axUNorm8::s_fromFloat(0) == axUNorm8(0));
		AX_TEST_CHECK(axUNorm16::s_fromFloat(0) == axUNorm16(0));

		AX_TEST_CHECK(axUNorm8::s_fromFloat(1) == axUNorm8(0xff));
		AX_TEST_CHECK(axUNorm16::s_fromFloat(1) == axUNorm16(0xffff));

		AX_TEST_CHECK(axUNorm8::s_fromFloat(0.5f)  == axUNorm8(127));
		AX_TEST_CHECK(axUNorm16::s_fromFloat(0.5f) == axUNorm16(32767));
	}
};

void test_axUNorm() {
	AX_TEST_CASE(Test_axUNorm, test_case1());
}