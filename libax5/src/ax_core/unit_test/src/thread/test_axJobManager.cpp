#include <ax_core/thread/axJobManager.h>

class Test_axJobManager : public axUnitTestClass {
	using This = Test_axJobManager;
public:
	axJobManager mgr;

	Test_axJobManager() {
		axJobManager_CreateDesc desc;
		desc.name = "test_axJobManager";
		desc.threadCount = axSystemInfo::cpuCount();

		mgr.create(desc);
	}

	~Test_axJobManager() {
		mgr.destroy();
	}

	static void onJob() {
		AX_LOG("onJob start");
		for (axInt i = 0; i < 10; i++) {
			ax_sleep(200);
			AX_LOG("onJob {?}", i);
		}
	}

	void test_case1() {
		auto desc = ([](){
			onJob();
		});
		auto job = mgr.addJob(AX_LOC, desc);

		AX_LOG("job wait...");
		while (!job.isDone()) {
			ax_sleep(200);
		}

		AX_LOG("job finish");
	}
};

void test_axJobManager() {
	AX_TEST_CASE(Test_axJobManager, test_case1());
}
