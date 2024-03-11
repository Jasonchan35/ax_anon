class Test_axEvent : public axUnitTestClass {
public:

	class MyObject : public axObject {
		AX_RTTI_CLASS(MyObject, axTypedBase);
	public:
		void method1(int a, float b) {
			AX_LOG("method1 {?} {?}", a, b);
		}
	};

	static void func1(int a, float b) {
		AX_LOG("func1 {?} {?}", a, b);
	}

	void test1() {
		axEvent<void (int, float)> e;
		{
			auto o = axSPtr_new(MyObject);

			e.bind(&func1);
			e.unsafeBind(o.ptr(), &MyObject::method1);
			e.weakBind(o, &MyObject::method1);
			e.bind(o.ptr(), &MyObject::method1);

			e.emit(8, 9);

			e.unbind(&func1);
			e.unbind(o, &MyObject::method1);
			e.unbindByObject(o);
		}
		e.emit(3, 4);
	};

	void test_enterOnce() {
		int called = 0;
		axEvent<void ()> ev;

		ev.bind(this, [&](){
			if (!AX_TEST_CHECK(called == 0))
				return;
			called++;
			ev.emit();
		});

		ev.emit();
	}

	void test_reenter() {
		int called = 0;
		axReentrantEvent<void ()> ev;

		ev.bind(this, [&](){
			if (called >= 5) return;
			called++;
			ev.emit();
		});

		ev.emit();
		AX_TEST_CHECK(called == 5);
	}
};

void test_axEvent() {
	AX_TEST_CASE(Test_axEvent, test_enterOnce());
	AX_TEST_CASE(Test_axEvent, test_reenter());
	AX_TEST_CASE(Test_axEvent, test1());
}
