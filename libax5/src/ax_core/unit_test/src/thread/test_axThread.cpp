#include <ax_core/profiler/axProfiler.h>

class Test_axThread : public axUnitTestClass {
public:
	axCondInt<false>	count{999};

	void onThreadProc() {
		for (axInt i = 0; i < 20; i++) {
			ax_sleep(50);
			AX_DUMP_VAR(++count);
		}
	}

	void test_case1() {
		AX_TEST_CHECK(count == 999);

	
		count.set(100);
		AX_TEST_CHECK(count == 100);
		
		axThread t0;
		t0.start("test_axThread", [this](){ onThreadProc(); });

		const axInt target = 110;
		AX_LOG("wait > {?} ...", target);
		{
			auto ret = count.waitGreaterThan(target);
			AX_LOG("wait > {?} return {?}", target, ret);
			AX_TEST_CHECK(ret > target);
		}

		t0.join();
		AX_TEST_CHECK(count == 120);

		axCondInt<false> vi;
		++vi;
	}
};

void test_axThread() {
 	AX_TEST_CASE(Test_axThread, test_case1());
}
