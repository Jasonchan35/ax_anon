
class Test_axSystemInfo : public axUnitTestClass {
public:
	void test_case1() {
		AX_LOG("OS = {?}\n", axSystemInfo::getOSName());
	}
};

void test_axSystemInfo() {
	AX_TEST_CASE(Test_axSystemInfo, test_case1());
}
