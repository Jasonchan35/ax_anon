
class Test_axSpan : public axUnitTestClass {
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
		TestData() = default;
		int data;
	};

	void func(axSpan<TestBase> s) {
	}

	void test_case1() {
		//AX_DUMP_VAR( (axTypeTraits::isConvertible<int*, float*>::value) );
		//AX_DUMP_VAR( (axTypeTraits::isConvertible<int*, const int*>::value) );
		//AX_DUMP_VAR( (axTypeTraits::isConvertible<const int*, int*>::value) );
		//AX_DUMP_VAR( (axTypeTraits::isConvertible<TestData*, const TestBase*>::value) );
		//AX_DUMP_VAR( (axTypeTraits::isConvertible<TestBase*, const TestData*>::value) );

		TestData obj;

		TestData data[10];
		axMutSpan<TestData> mutSpan(data);
		axSpan<TestBase> spanFromSingleObject(obj);

		auto mutByteSpan = mutSpan.toByteMutSpan();
		mutByteSpan[0] = 10;

		// auto byteSpan = span.toByteMutSpan();
		// byteSpan[0] = 10; // cannot compile because it's const byte
	}

};

void test_axSpan() {
	AX_TEST_CASE(Test_axSpan, test_case1());
}
