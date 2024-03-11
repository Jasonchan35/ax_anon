
#if axVec4f_SSE_Defined

class Test_axVec3f_SSE : public axUnitTestClass {
public:
	template<class V>
	struct TestCases {
		V a, b, c, d, e;
		float scalar;
		TestCases() {
			scalar = -2.333f;
			a.set(0,0,0);
			b.set(1,1,1);
			c.set(2,2,2);
			d.set(-1.5f, 0, 12);
			e.set(9, -2.3f, 3);
		}

		//----------
		V add() { return d + e; }
		V sub() { return d - e; }
		V mul() { return d * e; }
		V div() { return d / e; }
		//----------
		V add_scalar() { return d + scalar; }
		V sub_scalar() { return d - scalar; }
		V mul_scalar() { return d * scalar; }
		V div_scalar() { return d / scalar; }
		//----------
		V assign_add() { V o = d; o += e; return o; }
		V assign_sub() { V o = d; o -= e; return o; }
		V assign_mul() { V o = d; o *= e; return o; }
		V assign_div() { V o = d; o /= e; return o; }
		//----------
		V assign_add_scalar() { V o = d; o += scalar; return o; }
		V assign_sub_scalar() { V o = d; o -= scalar; return o; }
		V assign_mul_scalar() { V o = d; o *= scalar; return o; }
		V assign_div_scalar() { V o = d; o /= scalar; return o; }
	};

	using C = axVec3_Cpp<float>;
	using S = axVec3f_SSE;

	TestCases<C> tc;
	TestCases<S> ts;

	void check_sse(const char* name, const C& c, const S& s) {
		bool b = (c.x == s.x && c.y == s.y && c.z == s.z);
		AX_LOG("{?} {?}", ( b ? AX_STR("[OK ] ") : AX_STR("[ERR] ")), axStrView_c_str(name));
	}

	void test_case1() {
		#define compare_sse(TEST) \
			do{ \
				auto c = tc.TEST(); \
				auto s = ts.TEST(); \
				check_sse(#TEST, c, s); \
			}while(false) \
		//----------
			compare_sse(add);
			compare_sse(sub);
			compare_sse(mul);
			compare_sse(div);
			//------
			compare_sse(add_scalar);
			compare_sse(sub_scalar);
			compare_sse(mul_scalar);
			compare_sse(div_scalar);
			//------
			compare_sse(assign_add);
			compare_sse(assign_sub);
			compare_sse(assign_mul);
			compare_sse(assign_div);
			//------
			compare_sse(assign_add_scalar);
			compare_sse(assign_sub_scalar);
			compare_sse(assign_mul_scalar);
			compare_sse(assign_div_scalar);
		#undef compare_sse
	}
};

void test_axVec3f_SSE() {
	AX_TEST_CASE(Test_axVec3f_SSE, test_case1());
}

#else
	void test_axVec3f_SSE() {}
#endif
