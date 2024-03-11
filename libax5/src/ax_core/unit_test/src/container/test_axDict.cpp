
class Test_axDict : public axUnitTestClass {
public:

class Node {
public:
	Node(axInt v) : value(v) {}
	axInt value;
};

void test_case1() {
	axDict< axInt, Node > dict;

	for (axInt i=0; i<10; i++) {
		dict.add(i, Node(i*1000 + i));
	}

	//for(auto& p : dict) {
	//	std::cout << p.value << " ";		
	//}
	//std::cout << "\n";
}

};

void test_axInDict() {
	AX_TEST_CASE(Test_axDict, test_case1());
}
