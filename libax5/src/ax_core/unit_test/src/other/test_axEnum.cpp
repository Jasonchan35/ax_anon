
#define MyEnum_EnumList(EV) \
	EV(AAA,       ) \
	EV(B,   = 10  ) \
	EV(C,         ) \
	EV(D,         ) \
	EV(E,   = 2000) \
	EV(F,         ) \
	EV(GGG_GGG,) \
//----
ax_ENUM_CLASS(MyEnum, axUInt32);

class MyEnum2; // test c++ forward declaration

#define MyOption_EnumList(E) \
	E(A, = 1 << 10)\
	E(B, = static_cast<axInt16>(1U << 15) /* ignore warning for signed int16 test, because operator>> care signed/unsigned */ )\
	E(C, = 1 <<  7)\
	E(D, = 1 <<  1)\
//----
ax_ENUM_FLAGS_CLASS(MyOption, axInt16);

//enum class MyOption : axInt16; // test c++ forward declaration

class Test_axEnum : public axUnitTestClass {
public:
	void test_ax_ENUM_CLASS() {
		MyEnum a = MyEnum::B;
		AX_TEST_CHECK(MyEnum::B == a++);
		AX_TEST_CHECK(MyEnum::C ==   a);
		AX_TEST_CHECK(MyEnum::D == ++a);
		AX_TEST_CHECK(MyEnum::D ==   a);

		AX_DUMP_VAR(MyEnum::D);

		a = ax_enum(MyEnum::C);
		AX_TEST_CHECK(MyEnum::C == a);

		AX_LOG("\n-- MyEnum2 --");
		for (const auto& e : axEnum<MyEnum>::s_entries()) {
			AX_LOG("[{?}] = [{?}]", e.name, ax_enum_int(e.value));

			axEnum<MyEnum> tmp;
			AX_TEST_CHECK_SLIENT(tmp.tryParse(e.name));
			AX_TEST_CHECK_SLIENT(tmp == e.value);
		}
		AX_LOG("------\n");

		AX_TEST_CHECK(ax_enum_name(MyEnum::C) == "C");
	};

	void test_axEnumMask() {
		axTempString json;
		axJsonSerializer se(json);

		axEnum<MyOption> mask = MyOption::B | MyOption::C;
		mask.toggleFlags(MyOption::A);
		AX_TEST_CHECK(mask == (MyOption::A | MyOption::B | MyOption::C));

		mask.toggleFlags(MyOption::C);
		AX_TEST_CHECK(mask == (MyOption::A | MyOption::B));

		axTempString str;
		str.format("{?}", mask);
		AX_TEST_CHECK(str == "A | B");

		MyOption parseMask;
		if (AX_TEST_CHECK(str.tryParse(parseMask))) {
			AX_TEST_CHECK(parseMask == mask);
		}

		MyOption test;
		str.tryParse(test);

		se.io(mask);
		AX_DUMP_VAR(json);

		mask.setFlags(MyOption::B);
		mask.unsetFlags(MyOption::A);

		mask.setFlags(MyOption::C | MyOption::D);

		AX_TEST_CHECK(mask == (MyOption::B | MyOption::C | MyOption::D));

		AX_DUMP_VAR(mask.lowestBit());

		AX_TEST_CHECK(mask.count1Bits() == 3);
		AX_TEST_CHECK(mask.count0Bits() == 13);
		AX_TEST_CHECK((MyOption(1) << mask.highestBit()) == MyOption::B);
		AX_TEST_CHECK((MyOption(1) << mask.lowestBit() ) == MyOption::D);
	};
};

void test_axEnum() {
	AX_TEST_CASE(Test_axEnum, test_ax_ENUM_CLASS());
	AX_TEST_CASE(Test_axEnum, test_axEnumMask());
}
