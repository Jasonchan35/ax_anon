
#include <ax_core/time/axStopWatch.h>

class Test_axAllocator : public axUnitTestClass {
public:


	class TestData : public axDListNode<TestData> {
	public:
//		virtual ~TestData() {}

		int a;
//		char foo[160];
	};

	class TestThrow : public axError {};

	class TestDataThrowException {
	public:
		TestDataThrowException() {
			throw TestThrow();
		}

		int a;
	};

	void test_malloc() {
		auto* p = ax_malloc(axInt, 1000, 0);
		ax_free(p);
	}


	void test_new() {
		#if AX_USE_ALLOCATOR
		try{
			auto p = ax_new TestData;
			ax_delete(p);
		}catch(axAllocatorError&) {
			// ok!
		}catch(...){
			throw axError_Undefined(AX_LOC);
		}
		#endif

		TestData* o = nullptr;
		o = ax_new TestData;
		ax_delete(o);
	}

	void test_new_exception() {
		TestDataThrowException* o = nullptr;
		try {
			o = ax_new TestDataThrowException;
		}catch(axError&) {
		}
		ax_delete(o);
	}

	template<class T>
	void test_new_array() {
		T* arr = nullptr;
		AX_DUMP_VAR(ax_sizeof(T) * 500);
		arr = ax_new T[500];
		ax_delete_array(arr);
	}

	void _alloc_objects(axAllocator* allocator) {
		AX_DUMP_VAR(ax_sizeof(TestData));

		axDList<TestData> list;

		for (axInt i = 0; i < 1000000; i++) {
			auto o = axUPtr_new_(allocator, TestData);
			list.append(ax_move(o));
		}

		list.clear();
	}

	void test_alloc_objects_linear_allocator(bool b) {
		axStopWatch watch;

		axLinearAllocator allocator(ax_default_allocator(), 256 * 1024 * 1024);
		allocator.setUseLock(b);
		allocator.setTracking(b);
		allocator.setTailStampCheck(b);
		allocator.setFillMemoryWhenAlloc(b);
		allocator.setFillMemoryWhenDealloc(b);

		_alloc_objects(&allocator);
		AX_DUMP_VAR(watch.get());
//		AX_DUMP_VAR(allocator);
	}

	void test_alloc_objects() {
		axStopWatch watch;
		_alloc_objects(ax_default_allocator());
		AX_DUMP_VAR(watch.get());
	}
};

void test_axAllocator() {
	AX_TEST_CASE(Test_axAllocator, test_malloc());
	AX_TEST_CASE(Test_axAllocator, test_new());
//	AX_TEST_CASE(Test_axAllocator, test_new_exception());
	AX_TEST_CASE(Test_axAllocator, test_new_array<Test_axAllocator::TestData>());
	AX_TEST_CASE(Test_axAllocator, test_new_array<int>());

//	AX_TEST_CASE(Test_axAllocator, test_alloc_objects());
	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(true));
	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(true));
	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(true));

	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(false));
	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(false));
	AX_TEST_CASE(Test_axAllocator, test_alloc_objects_linear_allocator(false));

//	ax_root_allocator()->logInfo();
}
