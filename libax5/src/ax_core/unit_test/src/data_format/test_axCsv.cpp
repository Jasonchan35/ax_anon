#include <ax_core/data_format/csv/axCsvReader.h>

class Test_axCsv : public axUnitTestClass {
public:
	void test_case1() {
		auto data = axStrView(
R"(Id,Name,Level
1,Slime
"2","Zombie ABC,123",7,100)");

		AX_DUMP_VAR(data);

		axCsvReader reader(data, "");

		for (axInt i = 0; reader.nextRow(); i++) {
			AX_LOG("row{?} {?}", i, reader.row());
		}
	}
};

void test_axCsv() {
 	AX_TEST_CASE(Test_axCsv, test_case1());
}
