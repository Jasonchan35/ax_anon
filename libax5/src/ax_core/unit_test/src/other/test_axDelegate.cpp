class Test_axDelegate : public axUnitTestClass {
public:
	class MyObject : public axObject {
		AX_RTTI_CLASS(MyObject, axObject);
	public:
		void method1(int a, float b) {
			AX_LOG("method1 {?} {?}", a, b);
		}

		int v = 10;
	};

	static void func1(int a, float b) {
		AX_LOG("func1 {?} {?}", a, b);
	}

	void test1() {
		auto o = axSPtr_new(MyObject);

		axDelegate<void (int, float)> f;
		f.bind(&func1);
		f.emit(2,0);

		axDelegate<void (int, float)> m;
		m.unsafeBind(o.ptr(), &MyObject::method1);
		m.emit(3,3.1f);

		axDelegate<void (int)> s;
		int i = 100;
		s.emit(i);
	};

	void test2() {
		axDelegate<int (int)> del;
		{
			MyObject obj;
			del.bind(&obj, [&obj](int a) -> int{
				return obj.v + a;
			});
			AX_TEST_CHECK(del.emit(12) == 22);
		}

		{
			bool called = true;
			del.emitValid(called, 9);
			AX_TEST_CHECK(called == false);
		}
	}
};

void test_axDelegate() {
//	AX_TEST_CASE(Test_axDelegate, test1());
	AX_TEST_CASE(Test_axDelegate, test2());
}
