class Test_axHash : public axUnitTestClass {
public:
	void test1() {
		{
			auto h = axHash_get(10000);
			AX_DUMP_VAR(h);
		}
		{
			auto h = axHash_get(axStrView("ABC"));
			AX_DUMP_VAR(h);
		}
		{
			//const char* sz = "ABC";
			//auto h = axHash_get(sz); // shouldn't compile
		}
		{
			auto h = axHash_get("ABC");
			AX_DUMP_VAR(h);
		}
	}

	using MyHashString = axHashData<axString>;

	void test_HashData() {
		MyHashString a("ABC");
		MyHashString b("ABC");
		MyHashString c("EEE");

		AX_DUMP_VAR(a.hash());
		AX_DUMP_VAR(b.hash());
		AX_DUMP_VAR(c.hash());

		AX_TEST_CHECK(a == b);
		AX_TEST_CHECK(b != c);

		a.edit().set("EEE");
		AX_TEST_CHECK(a == c);
	}
};

void test_axHash() {
	AX_TEST_CASE(Test_axHash, test1());
	AX_TEST_CASE(Test_axHash, test_HashData());
}
