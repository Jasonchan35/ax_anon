class Test_axStrView : public axUnitTestClass {
public:

	void test_case1() {
		axInt i = 0;
		AX_TEST_CHECK(axStrView("10").tryParse(i));
		AX_TEST_CHECK(i == 10);

		AX_TEST_CHECK(axStrView("99a").tryParse(i) == false);
	}
};

void test_axStrView() {
	AX_TEST_CASE(Test_axStrView, test_case1());
}
