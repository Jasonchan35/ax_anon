void AX_VA_ARGS_macroGen() {
	axString o;
	const axInt n = 32;

	o.append("\n\n//====== AX_VA_ARGS_macroGen() from test_ax_macro.cpp ========\n\n");
	o.append("//! will always return >= 1, even AX_VA_ARGS_COUNT(), because it means one single empty arguemnt \n");
	o.append("#define AX_VA_ARGS_COUNT(...) AX_IDENTITY( AX_VA_ARGS_COUNT_INTERNAL(__VA_ARGS__");
	for (axInt i = 0; i < n; i++) {
		o.appendFormat(", {?}", n - i);
	}
	o.append(") )\n");

	o.append("#define AX_VA_ARGS_COUNT_INTERNAL(");
	for (axInt i = 0; i < n; i++) {
		o.appendFormat("a{?}, ", i);
	}
	o.append("COUNT, ...) COUNT\n\n");

	for (axInt i = 0; i < n; i++) {
		o.appendFormat("#define AX_VA_ARGS_FOR_EACH_{?}(MACRO, X", i+1);
		
		if (i > 0) {
			o.append(", ...");
		}
		o.append(")");
		o.appendFormat(" MACRO(X)");
		if (i > 0) {
			o.appendFormat(" AX_IDENTITY(AX_VA_ARGS_FOR_EACH_{0}(MACRO, __VA_ARGS__))", i);
		}
		o.append("\n");
	}
	o.append("\n//====== End AX_VA_ARGS_macroGen() ========\n\n");

	AX_LOG(o);
}

class Test_ax_macro : public axUnitTestClass {
public:
	axInt calledCount = 0;
	axInt funcToRunOnce() {
		AX_RUN_ONCE(++calledCount);
		return calledCount;
	}

	void test_AX_RUN_ONCE() {
		AX_TEST_CHECK(calledCount == 0);
		AX_TEST_CHECK(funcToRunOnce() == 1);
		AX_TEST_CHECK(calledCount == 1);
		AX_TEST_CHECK(funcToRunOnce() == 1);
		AX_TEST_CHECK(funcToRunOnce() == 1);
	}

	void test_AX_VA_ARGS_COUNT() {
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY()      == true);
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY( )     == true);
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY(a)     == false);
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY(,)     == false);
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY(a,b)   == false);
		AX_TEST_CHECK(AX_VA_ARGS_IS_EMPTY(a,b,c) == false);

		AX_TEST_CHECK(1 == AX_VA_ARGS_COUNT()); // yes it should be 1, because it means one single empty argument
		AX_TEST_CHECK(1 == AX_VA_ARGS_COUNT(a));
		AX_TEST_CHECK(2 == AX_VA_ARGS_COUNT(,));
		AX_TEST_CHECK(2 == AX_VA_ARGS_COUNT(a,));
		AX_TEST_CHECK(2 == AX_VA_ARGS_COUNT(,b));
		AX_TEST_CHECK(2 == AX_VA_ARGS_COUNT(a,b));
		AX_TEST_CHECK(3 == AX_VA_ARGS_COUNT(a,b,c));
		AX_TEST_CHECK(4 == AX_VA_ARGS_COUNT(a,b,c,d));
	}

	void test_AX_VA_ARGS_FOR_EACH() {
		#define MY_TEST(x)						AX_LOG("MY_TEST({?})", #x);
		AX_VA_ARGS_FOR_EACH(MY_TEST,  1, (2,a), (3,a,b,c), 4, 5, 6, 7, 8, 9)

		#define MY_TEST_PARAM_IMPL(x, param)	AX_LOG("MY_TEST_PARAM({?}, {?})", #x, #param);
		#define MY_TEST_PARAM(x)				MY_TEST_PARAM_IMPL(x, "foo")
		AX_VA_ARGS_FOR_EACH(MY_TEST_PARAM, 1, 2, 3)
	}

	void test_AX_DUMP_VAR() {
		AX_DUMP_VAR(1);
		AX_DUMP_VAR(1,2);
		AX_DUMP_VAR(1,2,3);
	}
};

void test_ax_macro() {
	AX_TEST_CASE(Test_ax_macro, test_AX_RUN_ONCE());
	AX_TEST_CASE(Test_ax_macro, test_AX_DUMP_VAR());
	AX_TEST_CASE(Test_ax_macro, test_AX_VA_ARGS_COUNT());
	AX_TEST_CASE(Test_ax_macro, test_AX_VA_ARGS_FOR_EACH());
//	AX_VA_ARGS_macroGen();
}
