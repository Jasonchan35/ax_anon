class Test_axMap : public axUnitTestClass {
public:

class Node : public axMapNode<axString, Node>, public axMapNode<axInt, Node> {
	using This = Node;
public:
	using NameMapNode = axMapNode<axString, This>;
	using IdMapNode   = axMapNode<axInt, This>;

	Node(axInt v) {
		_value = v;
	}

	const axInt&    id()	{ return   IdMapNode::mapNode_key; }
	const axString& name()  { return NameMapNode::mapNode_key; }

private:
	axInt	_value;
};

void test_case1() {
	axMap<axInt,    Node, ax_delete>	map; //owner
	axMap<axString, Node, ax_no_delete>	nameMap;

	Node* firstNode = nullptr;

	for (axInt i=0; i<10; i++) {
		auto* node = ax_new Node(i * 10);
		if (!firstNode) firstNode = node;
		map.add(i, node);

		axTempString tmp;
		tmp.format("Hello {?}", i);
		nameMap.add(tmp, node);

		AX_LOG("name = {?}", node->name());
	}
}

};

void test_axMap() {
	AX_TEST_CASE(Test_axMap, test_case1());
}
