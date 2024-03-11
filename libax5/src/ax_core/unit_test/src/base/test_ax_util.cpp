// #include "unit_test_dll.h"

#include <ax_core.h>
#include <ax_core/unit_test/axUnitTest.h>

class Test_ax_util : public axUnitTestClass {
public:

	class Obj {
	public:
		void test(int d) {
			printf("Obj::test %d\n", d);
		}
	};

	static void test_static_fn(int d) {
		printf("test_static_fn %d\n", d);
	}

	void test_instance_fn(int d) {
		printf("test_instance_fn %d\n", d);
	}

	void test_callback() {
		Obj o;

//		axFunc<void(int d)> fn1 = ax_bind(&Obj::test, o, ax_1); //member function
		axFunc<void(int d)> fn2 = [&o](int d) { o.test(d); };  //lambda to member function
		axFunc<void(int d)> fn3 = test_static_fn;
		axFunc<void(int d)> fn4 = [this](int d){ this->test_instance_fn(d); };

		//auto fn4 = ax_mem_fn(&Obj::test);
		//axFunc<void(int d)> fn5 = ax_mem_fn(&Obj::test);

//		fn1(11);
		fn2(12);
		fn3(13);
		//fn4(o, 14);
		//fn5(o, 15);
	}

	void test_defer_run() {
		axInt v = 10;
		{
			ax_defer([&v](){ 
				v += 1; 
			});
			v += 10;
			AX_TEST_CHECK( v == 20 );
		}
		AX_TEST_CHECK( v == 21 );
	}


	void test_field_count() {
		struct MySimpleType {
			int a = 11;
			int b = 22;
			float c;
		};
		AX_DUMP_VAR(ax_struct_field_count<MySimpleType>());
	}

/*
	void test_dll_static() {
		AX_DUMP_VAR(test_get_static());
		test_add_static(1);
		AX_DUMP_VAR(test_get_static());

		AX_DUMP_VAR(test_get_static2());
		test_add_static2(1);
		AX_DUMP_VAR(test_get_static2());
	}
	*/
};

void test_ax_util() {
	//AX_TEST_CASE(Test_ax_util, test_dll_static());
	//AX_TEST_CASE(Test_ax_util, test_callback());
	//AX_TEST_CASE(Test_ax_util, test_defer_run());
	//AX_TEST_CASE(Test_ax_util, test_field_count());
}
