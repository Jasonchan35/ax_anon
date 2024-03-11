
class Test_ax_math : public axUnitTestClass {
public:
	void test_case1() {
		for (axInt i = 0; i < 50; i++) {
			auto a = ax_next_pow2_half(i);
			auto b = ax_next_pow2(i);
//			AX_LOG("{?,3} = {?,3} {?,3}", i, a, b);
			AX_TEST_CHECK_SLIENT(a >= i);
			AX_TEST_CHECK_SLIENT(b >= i);
		}
	}

	void test_round() {
		AX_TEST_CHECK(ax_round( 1.4f) ==  1.0f);
		AX_TEST_CHECK(ax_round( 1.5f) ==  2.0f);
		AX_TEST_CHECK(ax_round(-1.4f) == -1.0f);
		AX_TEST_CHECK(ax_round(-1.5f) == -2.0f);
	}

	void test_inf() {
		float a = 10;

		AX_TEST_CHECK( ax_inf >   a);
		AX_TEST_CHECK( ax_inf ==  ax_inf);
		AX_TEST_CHECK( ax_inf >  -ax_inf);
		AX_TEST_CHECK(-ax_inf <   ax_inf);
		AX_TEST_CHECK(-ax_inf == -ax_inf);
		AX_TEST_CHECK(ax_nan  != -ax_nan);

		AX_TEST_CHECK(ax_min(a,  ax_inf) == a);
		AX_TEST_CHECK(ax_min(a, -ax_inf) != a);
	}
};

void test_ax_math() {
	AX_TEST_CASE(Test_ax_math, test_inf());
	AX_TEST_CASE(Test_ax_math, test_case1());
	AX_TEST_CASE(Test_ax_math, test_round());
}