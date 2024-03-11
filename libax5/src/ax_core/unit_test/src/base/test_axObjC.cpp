//
//  test_ax_ObjC.cpp
//  libax_core_unit_test
//
//  Created by Jason on 8/6/2020.
//

#if AX_OBJC

@interface TestObjC : NSObject {
}
@property () int a;
@end

@implementation TestObjC
- (id) debugQuickLookObject {
	return ax_toNSString("Hello");
}
@end


class TestObj {
public:
	id debugQuickLookObject() {
		return ax_toNSString("Hello");
	}

	int a;
};

#endif

class Test_axObjC : public axUnitTestClass {
public:
	void test_case1() {
#if AX_OBJC
		auto a = [TestObjC alloc];
		a.a = 10;

		TestObj b;
		b.a = 10;

		AX_DUMP_VAR(a.a, b.a);
#endif
	}
};

void test_axObjC() {
	AX_TEST_CASE(Test_axObjC, test_case1());
}
