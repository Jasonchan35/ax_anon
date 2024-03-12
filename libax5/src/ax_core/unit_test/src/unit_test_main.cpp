//
//  unit_test_main.cpp
//
//  Created by Jason on 2015-03-14.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#include <ax_core/unit_test/axUnitTest.h>

AX_OPTIMIZE_OFF

#define	RUN_TEST( fn )	\
	ax_unit_test_log("\n===== " #fn " ================================\n"); \
	void fn(); \
	fn(); \
//----
void run_temp_test() {
	RUN_TEST(test_axCsv);
}

void run_compiler_test() {
	RUN_TEST(test_compiler_issues);
}

void run_all_test() {
// base
	RUN_TEST(test_axHash);
	RUN_TEST(test_ax_macro);
	RUN_TEST(test_ax_util);
	RUN_TEST(test_axByteOrder);
	RUN_TEST(test_axObjC);
	RUN_TEST(test_axPushable);
	RUN_TEST(test_axTypeTraits);
// allocator
	RUN_TEST(test_axAllocator);
// container
	RUN_TEST(test_axArray);
	RUN_TEST(test_axArrayMap);
	RUN_TEST(test_axBitArray);
	RUN_TEST(test_axDList);
	RUN_TEST(test_axInDict);
	RUN_TEST(test_axMap);
	RUN_TEST(test_axSList);
	RUN_TEST(test_axSpan);
	RUN_TEST(test_axStackList);
	RUN_TEST(test_axTuple);
// data_format
	RUN_TEST(test_axBinSerializer);
	RUN_TEST(test_axCsv);
	RUN_TEST(test_axJson);
// file
	RUN_TEST(test_axPath);
	RUN_TEST(test_axDirectory);
// graph
	RUN_TEST(test_axColor);
// math
	RUN_TEST(test_ax_math);
	RUN_TEST(test_axUNorm);
	RUN_TEST(test_axVec3);
	RUN_TEST(test_axVec3f_SSE);
	RUN_TEST(test_axMat);
	RUN_TEST(test_axRect2);
	RUN_TEST(test_axBBox1);
// net
	RUN_TEST(test_axSocket);
	RUN_TEST(test_axNetEngine);
// other
	RUN_TEST(test_axDelegate);
	RUN_TEST(test_axEvent);
	RUN_TEST(test_axEnum);
	RUN_TEST(test_axUuid);
	RUN_TEST(test_axRandom);
// profiler
//	RUN_TEST(test_axProfiler)
// rtti
	RUN_TEST(test_axType);
// reflection
	RUN_TEST(test_axReflection);
// pointer
	RUN_TEST(test_axUPtr);
	RUN_TEST(test_axSPtr);
// string
	RUN_TEST(test_axString);
	RUN_TEST(test_axFormat);
	RUN_TEST(test_axNameId);
// system
	RUN_TEST(test_axSystemInfo);
// time
	RUN_TEST(test_axDateTime);
	RUN_TEST(test_axHiResTime);
// thread
	RUN_TEST(test_axThread);
	RUN_TEST(test_axJobManager);
}

int main() {
//	AX_DUMP_VAR(AX_COMPILER_VER);
	axError::s_setEnableAssertion(false);
	ax_core_init();

#if 1
	run_temp_test();
#else
	run_all_test();
#endif

//	run_compiler_test();
//	ax_root_allocator()->logInfoFile("test_axAllocatorInfo.json");
	ax_root_allocator()->checkMemoryLeak();

//--------
	ax_unit_test_log("\n\n==== Program Ended ==== \n");

	#if AX_COMPILER_VC
		if (IsDebuggerPresent()) {
			ax_unit_test_log(" ! Press Any Key to Exit\n\n");
			_getch();
		}
	#endif

	return 0;
}
