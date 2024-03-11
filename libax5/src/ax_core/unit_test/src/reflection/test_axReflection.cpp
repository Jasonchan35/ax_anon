#include <ax_core/reflection/axReflection.h>

class MyType {
public:
	int a = 10;
	int b = 20;
	float c = 1.1f;
	axString str {"test"};
	axVec2f vec2f {91, 92};

	AX_REFLECTION_BEGIN(MyType, axNoBase)
		ax_FIELDS(a, b, c, str, vec2f)
	AX_REFLECTION_END()
public:

	void onJsonWriter(axJsonWriter& wr) const {
		wr.reflection(*this);
	}

	void onFormat(axFormat& f) const {
		f.reflection(*this);
	}
};

namespace Test_namespace {
	class MyTypeInNamespace {
		int a = 10;
		int b = 20;
	public:
		AX_REFLECTION_BEGIN(MyTypeInNamespace, void)
			ax_FIELDS(a, b)
		AX_REFLECTION_END()
	};
}

class TestClass {
public:
	class MyTypeInClass {
		int a = 10;
		int b = 20;
		int k = 1;
	public:
		AX_REFLECTION_BEGIN(MyTypeInClass, void)
			ax_FIELDS(a, b, k)
		AX_REFLECTION_END()
	};

	class DerivedClass : public MyTypeInClass {
		int c = 30;
		int d = 40;

	public:
		AX_REFLECTION_BEGIN(DerivedClass, MyTypeInClass)
			ax_FIELDS(c, d)
		AX_REFLECTION_END()
	};

	template<class T>
	class MyTemplateTypeInClass {
		int a = 10;
		int b = 20;
	public:
		AX_REFLECTION_BEGIN(MyTemplateTypeInClass, void)
			ax_FIELDS(a, b)
		AX_REFLECTION_END()
	};
};

class Test_axReflection : public axUnitTestClass {
public:
	template<class T>
	void test_class() {
		using R = axReflection<T>;
		AX_DUMP_VAR(R::s_typeName());
		AX_LOG("\n");
	}

	void test1() {
		test_class<axVec2f>();
		test_class<TestClass::DerivedClass>();
		test_class<MyType>();
		test_class<TestClass::MyTypeInClass>();
		test_class<TestClass::MyTemplateTypeInClass<float> >();
		test_class<Test_namespace::MyTypeInNamespace>();
	}

	void test_json() {
		axString json;
		axJsonWriter wr(json);
		auto rootObject = wr.object();

		MyType t;
		t.a = 1000;
		t.onJsonWriter(wr);

		AX_DUMP_VAR(t);
		AX_DUMP_VAR(json);
	}

};

void test_axReflection() {
	AX_TEST_CASE(Test_axReflection, test_json());
//	AX_TEST_CASE(Test_axReflection, test1());
}
