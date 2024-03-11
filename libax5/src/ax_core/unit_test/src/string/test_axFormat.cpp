class Test_axFormat : public axUnitTestClass {
public:
	void test_format() {
		axString s;
		s.format("{?,5:0d} {?,6:+0x}", 10, 20);

		std::cout << "s = " << s << "\n";
		AX_TEST_CHECK(s == "00010 +00014");
	}

	void test_formatSelf() {
		axString s("ABC");
		try {
			s.appendFormat("{?}", s);
			AX_DUMP_VAR(s);
		} catch (axError_BufferOverlapped&) {
			return; // OK
		}
		AX_TEST_CHECK(false);
	}

	void test_toTempStr() {
		axString s;
		s.append(axTempString::s_convert(123), "ABC" );
		AX_TEST_CHECK(s == "123ABC");
	}

	void test_axFmtString() {
		auto s = axTempString::s_format("Test {?} {?}", "Hi", 123);
		AX_TEST_CHECK(s == "Test Hi 123");
	}
};

void test_axFormat() {
	AX_TEST_CASE(Test_axFormat, test_format());
	//AX_TEST_CASE(Test_axFormat, test_formatSelf());
	AX_TEST_CASE(Test_axFormat, test_toTempStr());
	AX_TEST_CASE(Test_axFormat, test_axFmtString());
}
