
class Test_axTypeTraits : public axUnitTestClass {
public:
	void test1() {
		AX_TEST_CHECK(axTypeTraits::isFloatingPoint<float>::value == true);
		AX_TEST_CHECK(axTypeTraits::isFloatingPoint<int>::value == false);
	}
};

void test_axTypeTraits() {
	AX_TEST_CASE(Test_axTypeTraits, test1());
}
