
class Test_axVec3 : public axUnitTestClass {
public:
	template<class T>
	void test_case1() {
		axArray<axVec2f> arr;
		arr.resize(20);

		auto m = axMat4<T>::kIdentity();
		auto m2 = m.transpose();
		AX_DUMP_VAR(m2);

		auto a = axVec3<T>(1,1,1);
		AX_DUMP_VAR(a);

		axVec3<T> v(1,2,3);
		v *= T(0.5);

		auto b = ax_min(v, axVec3<T>(T(1.1), T(1.1), T(1.1)));
		AX_DUMP_VAR(b);

		auto v3 = ax_lerp( axVec3<T>(1,1,1), axVec3<T>(0,0,0), (T)0.5);
		AX_DUMP_VAR(v3);
	}
};

void test_axVec3() {
	AX_TEST_CASE(Test_axVec3, test_case1<float>());
}