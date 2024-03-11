class Test_axJson : public axUnitTestClass {
public:
	void test_case1() {
		axString json;
		axJsonWriter wr(json);
		{
			auto root = wr.object();
			wr.writeMemberName("aa");
			wr.writeValue("123");
			wr.writeMember("bb", 456);

			auto arr = wr.array("arr");
			for (int i = 0; i < 5; i++) {
				wr.writeValue(i);
			}
		}

		AX_DUMP_VAR(json);

		axJsonReader rd(json, "");
		axString aa;
		axInt bb = -1;
		axArray<axInt> arr;
		{
			rd.beginObject();
			while (!rd.endObject()) {
				if (rd.isMember("bb")) {
					rd.readValue(bb);
					continue;
				}

				if (rd.readMember("aa", aa)) continue;

				if (rd.readMember("arr", arr)) continue;

				rd.skipUnhandledMember(true);
			}
		}

		AX_TEST_CHECK(aa == "123");
		AX_TEST_CHECK(bb == 456);

		axInt arrValue[] = {0, 1, 2, 3, 4};
		AX_TEST_CHECK(arr == arrValue);
	}

	void test_missingMemberName() {
		axString json;
		try{
			axJsonWriter wr(json);
			{
				auto obj = wr.object();
				wr.writeValue("123");
			}
		} catch (axJsonWriterError_MemberNameRequired&) {
			return; // OK
		} catch (...) {
			AX_TEST_CHECK(false);			
		}
		AX_TEST_CHECK(false);
	}

	void test_jsonValue() {
		AX_DUMP_VAR(sizeof(axJsonValue));
		AX_DUMP_VAR(sizeof(axJsonObject));
		AX_DUMP_VAR(sizeof(axJsonArray));

		axJsonValue json;
		{
			auto& obj = *json.setObjectType();
			obj["m_name"] = "John";
			obj["m_bool0"] = false;
			obj["m_bool1"] = true;
			obj["m_int"] = 10;
			obj["m_float"] = 3.4f;
			obj["m_double"] = 1.23;
			obj["m_null"] = nullptr;
			for (auto& d : *obj["m_array"].setArrayType(2)) {
				d.setValue(L"Unicode");
			}
		}
		AX_DUMP_VAR(json);
	}

	void test_array() {
		axIntArray arr;
		for (axInt i = 0; i < 10; i++) {
			arr.append(i);
		}

		axString json;
		axJsonSerializer se(json);
		se.io(arr);
		AX_DUMP_VAR(json);
	}

	void test_string() {
		axString_<200> tmp("testing");

		axString json;
		axJsonSerializer se(json);
		se.io(tmp);
		AX_DUMP_VAR(json);
	}
};

void test_axJson() {
	AX_TEST_CASE(Test_axJson, test_case1());
	//AX_TEST_CASE(Test_axJson, test_missingMemberName());
	//AX_TEST_CASE(Test_axJson, test_jsonValue());
	//AX_TEST_CASE(Test_axJson, test_array());
	//AX_TEST_CASE(Test_axJson, test_string());
}
