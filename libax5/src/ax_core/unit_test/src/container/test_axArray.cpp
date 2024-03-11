
class Test_axArray : public axUnitTestClass {
public:

	class TestBase {
	public:
		TestBase() = default;
		TestBase(int a) : v(a) {}
		int v;
	};

	class TestData : public TestBase {
		typedef TestBase Base;
	public:
	//	TestData() = default;
		TestData(const char* /* s */, int b) : Base(b) {}
	};

	void test_appendNew() {
		axArray<TestBase> a;
		a.emplaceBack();
		a.emplaceBack(1);

		a.resize(10);
		a.resize(20, 99); // cannot resize because TestBase constructor need paramter when new
	}

	void test_case1() {
		axArray<axInt> a;
		a.append(33);
		a.append(44);
		AX_DUMP_VAR(a);

		a.resize(10);

		axInt i=0;
		for(auto& p : a ) {
			p = i * 10;
			i++;
		}

		AX_DUMP_VAR(a);

	//-----------------
		const axInt c_array[] = {1,3,5,7,9};
		auto view  = axSpan_make( c_array );
		a.appendRange(view);
		a.appendRange(c_array);
		AX_DUMP_VAR(a);

	//---------------------
		test_appendNew();
	}

	void test_noInit() {
		axArray<axInt> intArray;
		intArray.resize(5);
		intArray.resize(10, axTag::NoInit);
		AX_DUMP_VAR(intArray);


		axArray<axVec3f> vec3Array;
		vec3Array.resize(5);
		vec3Array.resize(10, axTag::NoInit);
		AX_DUMP_VAR(vec3Array);

		axArray<axString> strArray;
		strArray.resize(5);
		strArray.resize(10, axTag::NoInit);
		AX_DUMP_VAR(strArray);
	}

};

void test_axArray() {
	AX_TEST_CASE(Test_axArray, test_noInit());
	AX_TEST_CASE(Test_axArray, test_case1());
}
