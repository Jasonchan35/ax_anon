
class Test_axType : public axUnitTestClass {
public:

	class MyTypeA : public axTypedBase {
		AX_RTTI_CLASS(MyTypeA, axTypedBase);
	public:
		virtual axStrLiteral name() { return "A"; }
	};

	class MyTypeB : public MyTypeA {
		AX_RTTI_CLASS(MyTypeB, MyTypeA);
	public:
		virtual axStrLiteral name() override { return "B"; }
	};

	class MyTypeC : public MyTypeB {
		AX_RTTI_CLASS(MyTypeC, MyTypeB);
	public:
		virtual axStrLiteral name() override { return "C"; }
	};

	void test1() {
		AX_DUMP_VAR(ax_typeof<MyTypeA>());
		AX_DUMP_VAR(ax_typeof<axVec3f>());
	};

	void test_axTypeFactory() {
		using Factory = axTypeFactoryT<MyTypeA>;
		Factory f;
		f.addEntry<MyTypeA>();
		f.addEntry<MyTypeB>();
		f.addEntry<MyTypeC>();

		{
			axUPtr<MyTypeA> p = f.createByName(AX_LOC, nullptr, "MyTypeC");
			AX_TEST_CHECK(p != nullptr);
			AX_TEST_CHECK(p->name() == "C");
		}

		{
			axUPtr<MyTypeA> p = f.create(AX_LOC, nullptr, ax_typeof<MyTypeC>());
			AX_TEST_CHECK(p != nullptr);
			AX_TEST_CHECK(p->name() == "C");
		}
	}

	void test_Vec2() {
		auto* t = ax_typeof<axVec2f>();
		AX_DUMP_VAR(t);
		//for (auto& f : t->fields()) {
		//	AX_DUMP_VAR(f);
		//}

		axVec2f v{11, 22};
		AX_DUMP_VAR(v);

		AX_LOG("\n");
	}
};

void test_axType() {
	AX_TEST_CASE(Test_axType, test1());
	AX_TEST_CASE(Test_axType, test_Vec2());
	AX_TEST_CASE(Test_axType, test_axTypeFactory());
}
