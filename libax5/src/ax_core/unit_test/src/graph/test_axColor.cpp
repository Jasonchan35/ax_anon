
class Test_axColor : public axUnitTestClass {
public:

	void test_toPremultipliedAlpha() {
		auto c = axColorRGBAb(100, 255, 0, 55);

		auto a = c.toPremultipliedAlpha();
		auto b = axColorRGBAb_make(axColorRGBAf_make(c).toPremultipliedAlpha());

		auto expected = axColorRGBAb(21, 55, 0, 55);
		AX_TEST_CHECK(a == expected);
		AX_TEST_CHECK(a == b);
	}

	void test_convert() {
		axColorRGBAb src(30, 100, 44);
		auto dst = axColorLb_make(src);
		AX_TEST_CHECK(dst.luma == 72);
	}

};

void test_axColor() {
	AX_TEST_CASE(Test_axColor, test_toPremultipliedAlpha());
	AX_TEST_CASE(Test_axColor, test_convert());
}