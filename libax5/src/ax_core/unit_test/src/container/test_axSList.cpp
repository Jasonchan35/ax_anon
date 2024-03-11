class Test_axSList : public axUnitTestClass {
public:
	class ListEvent2;

	class Node 
		: public axSListNode<Node>
		, public axSListNode<Node, ListEvent2> 
	{
		using This = Node;
	public:
		using ListNode  = axSListNode<This>;
		using ListNode2 = axSListNode<This, ListEvent2>;

		Node(axInt v) 
			: value(v) 
		{
		}
		axInt value;
	};

	class ListEvent2 : public axSListEventBase {
	public:
		static void onRemove(Node* p) {
			AX_LOG("remove2");
		}
	};

	using List  = axSList<Node>;
	using List2 = axSList<Node, ax_no_delete, Node::ListNode2>;

	template<class LIST>
	void test_dump(LIST& list) {
		for(auto& p : list) {
			std::cout << p.value << " ";
		}
		std::cout << "\n";
	}

	void test_verify(List& list, const axSpan<axInt>& arr) {
		axInt i = 0;
		for(auto& p : list) {
			AX_TEST_CHECK_SLIENT(p.value == arr[i]);
			i++;
		}
	}

	void test_case1() {	
		List list;
		List2 list2;
		axInt n = 6;

		for (axInt i=0; i<n; i++) {
			auto* node = ax_new Node(i);
			list.insert(node);
			list2.append(node);
		}

		test_dump(list);
		test_dump(list2);

		//test
		{
			axInt i = n-1;
			for(auto& p : list) {
				AX_TEST_CHECK_SLIENT(p.value == i);
				i--;
			}
		}

		{
			axInt i = 0;
			for(auto& p : list2) {
				AX_TEST_CHECK_SLIENT(p.value == i);
				i++;
			}
		}
	}

	void test_case2() {
		List list;

		auto first =list.insert(ax_new Node(0));
					list.insert(ax_new Node(1));
					list.insert(ax_new Node(2));
		auto mid  =	list.insert(ax_new Node(3));
					list.insert(ax_new Node(4));
		auto last =	list.insert(ax_new Node(5));

		list.insertAfter 		(ax_new Node(91), first);
		list.insertBeforeSlow	(ax_new Node(92), mid);
		list.insertAfter 		(ax_new Node(93), last);

		test_dump(list);
		axInt arr[] = {5,93,4,92,3,2,1,0,91};
		test_verify(list, arr);	

		List list2(ax_move(list));
	}

};

void test_axSList() {
	AX_DUMP_VAR(ax_sizeof(Test_axSList::List));
	AX_DUMP_VAR(ax_sizeof(Test_axSList::Node));
	AX_DUMP_VAR(ax_sizeof(Test_axSList::Node::ListNode));
	AX_DUMP_VAR(ax_sizeof(Test_axSList::Node::ListNode2));

	AX_TEST_CASE(Test_axSList, test_case1());
	AX_TEST_CASE(Test_axSList, test_case2());
}
