
class Test_axPath : public axUnitTestClass {
public:
	void test_dirs() {
		AX_DUMP_VAR(axPath::userHomeDir());
		AX_DUMP_VAR(axPath::userAppDataDir());
		AX_DUMP_VAR(axPath::userDocumentDir());
		AX_DUMP_VAR(axPath::userDesktopDir());
		AX_DUMP_VAR(axPath::appResourcesDir());

		{
			axTempString currentDir;
			axPath::getCurrentDir(currentDir);
			AX_DUMP_VAR(currentDir);
		}
	}

	void test_prefix() {
		AX_TEST_CHECK(axPath::hasPrefix("c:/aaa/bbb", "c:/aaa") == true);
		AX_TEST_CHECK(axPath::hasPrefix("c:/aaa/bbb", "c:/aa" ) == false);
	}
};

void test_axPath() {
	AX_TEST_CASE(Test_axPath, test_dirs());
	AX_TEST_CASE(Test_axPath, test_prefix());
}
