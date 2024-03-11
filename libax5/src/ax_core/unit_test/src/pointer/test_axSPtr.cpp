class Test_axSPtr : public axUnitTestClass {
public:
	class TestObject : public axObject {
	public:
		TestObject(int a_, int b_) 
			: a(a_)
			, b(b_) 
		{}

		int a;
		int b;
	};

	void test_weakPtr() {
		axWeak<TestObject>	w;

		{
			auto obj1 = axSPtr_new(TestObject(1,2));
			w.ref(obj1);

			auto obj2 = axSPtr_new(TestObject(3,2));
			w.ref(obj2);

			auto r = w.getSPtr();
			r->a = 10;
		}

		{
			auto r = w.getSPtr();
			AX_TEST_CHECK(r.ptr() == nullptr);
		}
	}

	void test_upcast() {
		auto obj1 = axSPtr_new(TestObject(4,2));

		axSPtr<axObject> b(obj1);
		axSPtr<axObject> c;
		c = obj1;
	}
};

void test_axSPtr() {
	AX_TEST_CASE(Test_axSPtr, test_weakPtr());
	AX_TEST_CASE(Test_axSPtr, test_upcast());
}
