
class Test_axBBox1 : public axUnitTestClass {
public:
	template<class T>
	void test_case1() {
		axBBox1<T> a;

		axBBox1<T> b(10, 30);
		a.include(b);
		AX_TEST_CHECK_SLIENT(a.min == 10 && a.max == 30);

		axBBox1<T> c(11, 35);
		a.include(c);
		AX_TEST_CHECK_SLIENT(a.min == 10 && a.max == 35);

		axBBox1<T> invalidBox;
		a.include(invalidBox);
		AX_TEST_CHECK_SLIENT(a.min == 10 && a.max == 35);
	}
};

void test_axBBox1() {
	AX_TEST_CASE(Test_axBBox1, test_case1<axInt>());
	AX_TEST_CASE(Test_axBBox1, test_case1<float>());
}