class Test_axString : public axUnitTestClass {
public:

	void foo(const axString& v) {
	}

	void test_case1() {
//		foo("123");

		//AX_LOG("test = {?}", u8"お疲れ様です。");
		//AX_LOG("test = {?}", u"お疲れ様です。");
		//AX_LOG("test = {?}", L"お疲れ様です。");

		AX_LOG("AX_FILE = {?}", AX_FILE);
		AX_LOG("ax_FILE_BASENAME = {?}", AX_FILE_BASENAME);

		axStringW w0;
		w0 = L"XVC";
		const auto& s0 = AX_STR( "abcd" );

		AX_DUMP_VAR(s0);

		axString_<10> s1;
		AX_LOG("{?}", s1);

		for( int i=0; i<20; i++ ) {
			s1.append( (axChar)('a' + i) );
		}

		s1.append( AX_STR("+0123") );
		s1.append(" GOOD! ");
		s1.append("a", "b", 'c');

		AX_DUMP_VAR(s1);
	}

	void test_persistString() {
		auto s1 = axPersistString::s_make("testing");
		auto s2 = axPersistString::s_make("ABC123");
		auto s3 = axPersistString::s_make("testing");
		AX_DUMP_VAR(s1,s2,s3);

		AX_TEST_CHECK(s1 == s3);
	}

	void test_convert() {
		axString s;
		s.convert(100).appendConvert(true);
		AX_DUMP_VAR(s);
		AX_TEST_CHECK(s == "100true");
	}

	void test_replace() {
		{
			axString s("Hello 123 abc");
			s.replaceAll("abc", "45678", true);
			AX_TEST_CHECK(s == "Hello 123 45678");
		}
		{
			axString s("Hello 123 abc");
			s.replaceAll("123", "45", true);
			AX_TEST_CHECK(s == "Hello 45 abc");
		}
		{
			axString s("Hello 123 abc");
			s.replaceAll("123", "45678", true);
			AX_TEST_CHECK(s == "Hello 45678 abc");
		}
	}

	template<class CH>
	void test_IString(axIStringT<CH>& t) {
		AX_DUMP_VAR(t);
	}

	template<class CH>
	void test_smallStringOptimization() {
		axInt n = 4000;
		axStringT<CH> s, t;

		for (axInt i = 0; i < n; i++) {
			CH c = 'a' + (i % 26);
			s.append(c);
			//printf("size=%lld, cap=%lld", s.size(), s.capacity());
			//printf("\n");
			AX_TEST_CHECK_SLIENT(s.size() == i + 1);
		}

		axString a("abc");
		AX_DUMP_VAR(a, s.capacity());

		test_IString(s);

		auto view = s.view();
		AX_DUMP_VAR(view);
		t = ax_move(s);

		for (axInt i = 0; i < n; i++) {
			CH c = 'a' + (i % 26);
			AX_TEST_CHECK_SLIENT(t[i] == c);
		}
	}
};

void test_axString() {
	AX_TEST_CASE(Test_axString, test_smallStringOptimization<char>());
	AX_TEST_CASE(Test_axString, test_smallStringOptimization<char16_t>());
	AX_TEST_CASE(Test_axString, test_smallStringOptimization<wchar_t>());
	AX_TEST_CASE(Test_axString, test_smallStringOptimization<char32_t>());
	AX_TEST_CASE(Test_axString, test_case1());
	AX_TEST_CASE(Test_axString, test_persistString());
	AX_TEST_CASE(Test_axString, test_convert());
	AX_TEST_CASE(Test_axString, test_replace());
}
