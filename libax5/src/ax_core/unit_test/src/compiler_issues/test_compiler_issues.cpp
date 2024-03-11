
class Test_CompilerIssues : public axUnitTestClass {
public:
	
	template<class T, float T::*ptrToField>
	class ListT {
	public:
		ListT() {
//			AX_STATIC_ASSERT(axTypeTraits::isPolymorphic<T>::value);
		}
	
		static axInt getOffset() {
			return axPtrUtil::memberOffset(ptrToField);
		}
	};

	class Node {
	public:
		virtual void func() {}

		int a;

		float v;

		static axInt test() {
			return axPtrUtil::memberOffset(&Node::v);
		}

		using List = ListT<Node, &Node::v>; // <-- getting member offset here could be wrong on Visual C++, 
		// because the Node class declaration is not done yet, so it didn't count the offset affected by v-table

		List list;
	};

	using List = ListT<Node, &Node::v>;

	Node node;
	List list;

	void test_declare_ptrToField() {
		AX_DUMP_VAR(ax_sizeof(Node));

		AX_DUMP_VAR(List::getOffset());
		AX_DUMP_VAR(Node::List::getOffset());

		//! FAIL in Visual C++
//		AX_TEST_CHECK(List::getOffset() == Node::List::getOffset());
	}
};

void test_compiler_issues() {
	AX_TEST_CASE(Test_CompilerIssues, test_declare_ptrToField());
}