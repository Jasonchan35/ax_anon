class Test_axArrayMap : public axUnitTestClass {
public:

struct Node {
	axInt a = 0;
	axInt b = 0;
};

void test_case1() {
	axArrayMap<axInt, Node>	map;

	const axInt N = 1000;

	for (axInt i=0; i<N; i++) {
		auto& node = map.add(i * 10);
		node.a = i * 100;
		node.b = i * 1000;
	}

	{
		for (axInt i=0; i<N; i++) {
			auto* e = map.find(i * 10);
			if (!AX_TEST_CHECK_SLIENT(e != nullptr))
				continue;

			AX_DUMP_VAR(e->a, e->b);
			AX_TEST_CHECK_SLIENT(e->a == i * 100);
			AX_TEST_CHECK_SLIENT(e->b == i * 1000);
		}
	}
}

};

void test_axArrayMap() {
	AX_TEST_CASE(Test_axArrayMap, test_case1());
}
