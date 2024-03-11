#include <ax_core/container/axTuple.h>

class Test_axTuple : public axUnitTestClass {
public:

	struct ForEachType_Handler {
		template<axInt index, class Field>
		void handle() {
			AX_DUMP_VAR(index, ax_typeof<Field>()->name());
		}
	};


	void test_case1() {
		auto tupleA = axTuple_make(100U, 1.23, 4.5f, -10, axStrView("test"), axString("abc"));
		AX_DUMP_VAR(tupleA);

		auto tupleB = axTuple_make(400, 500);
		AX_DUMP_VAR(tupleB);
		AX_TEST_CHECK(tupleB.get<0>() == 400);
		AX_TEST_CHECK(tupleB.get<1>() == 500);


		auto tupleAB = tupleA.join(tupleB);
		AX_DUMP_VAR(tupleAB);

		AX_TEST_CHECK(tupleAB.get<0>() == 100U);
		AX_TEST_CHECK(tupleAB.get<1>() == 1.23);
		AX_TEST_CHECK(tupleAB.get<2>() == 4.5f);
		AX_TEST_CHECK(tupleAB.get<3>() == -10);
		AX_TEST_CHECK(tupleAB.get<4>() == "test");
		AX_TEST_CHECK(tupleAB.get<5>() == "abc");
		// from B
		AX_TEST_CHECK(tupleAB.get<6>() == 400);
		AX_TEST_CHECK(tupleAB.get<7>() == 500);

		tupleAB.forEach([](auto index, const auto& value){
			AX_DUMP_VAR(value);
		});

		ForEachType_Handler h;
		decltype(tupleAB)::s_forEachType(h);
	}
};

void test_axTuple() {
	AX_TEST_CASE(Test_axTuple, test_case1());
}
