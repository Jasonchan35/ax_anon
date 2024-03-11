
class Test_axDirectory : public axUnitTestClass {
public:
	void test_case1() {
		axArray< axDirectoryEntry > arr;
		arr.resize(200);

		axDirectory::getFiles( arr, ".", false, false );	
	}
};

void test_axDirectory() {
	AX_TEST_CASE(Test_axDirectory, test_case1());
}
