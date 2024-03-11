class Test_axBitArray : public axUnitTestClass {
public:

	void test_case1() {
		const axInt n = 64 * 20 + 1;
		const axInt t = 7;

		axBitArray	arr;
		arr.resize(n);
		AX_DUMP_HEX(arr.byteSpan());

		for (axInt i = 0; i < n; i++) {
			auto v = i % t == 0;
			arr[i] = v;
		}
		AX_DUMP_VAR(arr);
		AX_DUMP_HEX(arr.byteSpan());

		arr.resize(n + 1);

		for (axInt i = 0; i < n; i++) {
			AX_TEST_CHECK_SLIENT(arr[i] == (i % t == 0));
		}

		arr[100] =  true;
		AX_TEST_CHECK_SLIENT(arr[100]);

		arr.resize(n * 3);

		arr[100] = false;
		AX_TEST_CHECK_SLIENT(!arr[100]);

		arr[100].toggle();
		AX_TEST_CHECK_SLIENT(arr[100]);

		AX_DUMP_HEX(arr.byteSpan());
	}

	void test_case2() {
		const axInt N = 128;
		const axInt t = 7;

		axFixedBitArray<N> arr;
		auto size = ax_sizeof(arr);
		AX_TEST_CHECK(size * 8 == N);

		for (axInt i = 0; i < N; i++) {
			auto b = i % t == 0;
			arr[i] = b;
		}

		for (axInt i = 0; i < N; i++) {
			auto b = i % t == 0;
			AX_TEST_CHECK_SLIENT(arr[i] == b);
		}
	}
};

void test_axBitArray() {
	AX_TEST_CASE(Test_axBitArray, test_case1());
	AX_TEST_CASE(Test_axBitArray, test_case2());
}
