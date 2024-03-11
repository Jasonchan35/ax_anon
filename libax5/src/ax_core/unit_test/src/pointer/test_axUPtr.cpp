
class Test_axUPtr : public axUnitTestClass {
public:
	class TestData {
	public:
		int a;
		int b;
	};

	void test_case1() {
		//auto* p = ax_new TestData();
		//axUPtr<TestData> uptr(p);
		auto uptr2 = axUPtr_new(TestData());
	}
};

void test_axUPtr() {
	AX_TEST_CASE(Test_axUPtr, test_case1());
}
