#include <ax_core/profiler/axProfiler.h>

class Test_axProfiler : public axUnitTestClass {
public:

	void test1(axInt N, axInt delay) {
		axProfiler::s_instance()->enableDebugPort();

		ax_PROFILER_CPU_TIMING("test1");
		ax_sleep(delay);

		axThread thread;
		thread.start("test_axProfiler", [N, delay](){
			ax_PROFILER_CPU_TIMING("thread");

			for (axInt i = 0; i < N; i++) {
				ax_PROFILER_CPU_TIMING("thread.loop");
				AX_LOG("thread.loop {?}", i);
				ax_sleep(delay);
			}
		});

		for (axInt i = 0; i < N; i++) {
			ax_PROFILER_CPU_TIMING("test1.loop");
			AX_LOG("test1.loop {?}", i);
			ax_sleep(delay);
		}
	}
};

void test_axProfiler() {
//	AX_TEST_CASE(Test_axProfiler, test1(10, 50));
	AX_TEST_CASE(Test_axProfiler, test1(100000000, 2000));
}
