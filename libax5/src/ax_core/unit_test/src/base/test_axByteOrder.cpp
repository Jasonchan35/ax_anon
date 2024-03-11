
class Test_axByteOrder : public axUnitTestClass {
public:
	void test1() {

	auto a = axFourCC('A','B','C','D');

	switch(a) {
		case axFourCC("ABCD"): break;
		default:
			AX_TEST_CHECK(false);
		break;
	}

	#if AX_CPU_ENDIAN_LITTLE
		{
			axUInt16 v = 0xABCD;
			auto o = axByteOrder::BigEndian::toHost(v);
			AX_LOG("{?,8:0X} -> {?,8:0X}", v, o);
			AX_TEST_CHECK(o == 0xCDAB);
		}
		{
			axUInt32 v = 0xABCD1234UL;
			auto o = axByteOrder::BigEndian::toHost(v);
			AX_LOG("{?,8:0X} -> {?,8:0X}", v, o);
			AX_TEST_CHECK(o == 0x3412CDAB);
		}
		{
			axUInt64 v = 0xABCD123456781A2BULL;
			auto o = axByteOrder::BigEndian::toHost(v);
			AX_LOG("{?,8:0X} -> {?,8:0X}", v, o);
			AX_TEST_CHECK(o == 0x2B1A78563412CDAB);
		}
	#endif
	};
};

void test_axByteOrder() {
	AX_TEST_CASE(Test_axByteOrder, test1());
}
