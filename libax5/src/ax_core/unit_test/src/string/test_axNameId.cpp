#include <ax_core/string/axNameId.h>

class Test_axNameId : public axUnitTestClass {
public:
	void test_case1() {
		auto a = axNameId::s_make("Testing123-45");
		AX_DUMP_VAR(a, a.name(), a.id());
		AX_TEST_CHECK(a.name() == "Testing123-" && a.id() == 45);

		auto b = axNameId::s_make("ABCD");
		AX_DUMP_VAR(b, b.name(), b.id());
		AX_TEST_CHECK(b.name() == "ABCD" && b.hasNoId());

		auto c = axNameId::s_make("Testing123-45");
		AX_DUMP_VAR(c, c.name(), c.id());
		AX_TEST_CHECK(c.name().str().data() == a.name().str().data());
		AX_TEST_CHECK(c.name().str().size() == a.name().str().size());


		axString tmp;
		a.getString(tmp);
		AX_TEST_CHECK(tmp == "Testing123-45");

		b.getString(tmp);
		AX_TEST_CHECK(tmp == "ABCD");

		AX_TEST_CHECK(a != b);
		AX_TEST_CHECK(a == c);
	}
};

void test_axNameId() {
	AX_TEST_CASE(Test_axNameId, test_case1());
}
