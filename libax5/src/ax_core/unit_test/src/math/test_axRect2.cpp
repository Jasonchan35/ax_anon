
class Test_axRect2 : public axUnitTestClass {
public:
	template<class T>
	void test_case1() {
		axRect2<T> v(1,2,10,20);

		auto c = v.area();
		AX_LOG("{?}", c);
	}
};

void test_axRect2() {
	AX_TEST_CASE(Test_axRect2, test_case1<float>());
}