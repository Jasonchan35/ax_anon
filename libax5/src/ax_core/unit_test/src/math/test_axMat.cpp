#include <ax_core/math/axMat.h>

class Test_axMat : public axUnitTestClass {
public:
	template<class T>
	void test_case1() {
		axMat<float> m;
		m.create(6, 4);

		for (axInt r = 0; r < m.numRow(); r++) {
			for (axInt c = 0; c < m.numCol(); c++) {
				m.at(c, r) = c * 10.0f + r;
			}
		}

		AX_DUMP_VAR(m);
	}

	template<class T>
	void test_case2() {
		axMat<float> a, b, c;
		a.create(3, 2);
		b.create(2, 3);

		a.row(0).setValues({1, 2, 3});
		a.row(1).setValues({4, 5, 6});

		b.row(0).setValues({7,  8});
		b.row(1).setValues({9,  10});
		b.row(2).setValues({11, 12,});

		c.mul(a, b);

		axMat<float> expected(
			{{58,  64},
			{139, 154}}
		);

		//AX_DUMP_VAR(a);
		//AX_DUMP_VAR(b);
		//AX_DUMP_VAR(c);
		AX_DUMP_VAR(expected);
		AX_TEST_CHECK(c == expected);
	}
};

void test_axMat() {
//	AX_TEST_CASE(Test_axMat, test_case1<float>());
	AX_TEST_CASE(Test_axMat, test_case2<float>());
}