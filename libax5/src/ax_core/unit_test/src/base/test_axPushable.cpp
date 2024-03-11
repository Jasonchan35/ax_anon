
class Test_axPushable : public axUnitTestClass {
public:

	axPushable<axInt>	i = 1000;

	void test_push() {
		axInt v = 100;
		{
			auto s = ax_push(v);
			s = 200;
			AX_TEST_CHECK(v == 200);
		}
		{
			auto s = ax_push(v);
			v = 300;
			AX_TEST_CHECK(v == 300);
		}
		AX_TEST_CHECK(v == 100);

		{
			auto p = i.push();
			p += 10;
			AX_TEST_CHECK(i.get() == 1010);
		}
		AX_TEST_CHECK(i.get() == 1000);
	}
};

void test_axPushable() {
	AX_TEST_CASE(Test_axPushable, test_push());
}
