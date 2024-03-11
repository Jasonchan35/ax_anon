#include <ax_core/data_format/axBinDeserializer.h>

class Test_axBinSerializer : public axUnitTestClass {
public:

	template<class T>
	void test_int_fixed_all() {
		const auto kMin = axNumLimit<T>::min();
		const auto kMax = axNumLimit<T>::max();

		axByteArray buf;
		{
			axBinSerializer se(buf);
			T i = kMin;
			for (;;) {
				T value = i;
				se.io_fixed(value);
				if (i == kMax)
					break;
				i++;
			}
		}

		{
			axBinDeserializer se(buf);
			T i = kMin;
			for (;;) {
				T value;
				se.io_fixed(value);
				//AX_DUMP_VAR(value);
				AX_TEST_CHECK_SLIENT(value == i);

				if (i == kMax)
					break;
				i++;
			}
		}
	}

	template<class T>
	void test_int_vary_all() {
		const auto kMin = axNumLimit<T>::min();
		const auto kMax = axNumLimit<T>::max();

		axByteArray buf;
		{
			axBinSerializer se(buf);
			T i = kMin;
			for (;;) {
				T value = i;
				se.io(value);
				if (i == kMax)
					break;
				i++;
			}
		}

		{
			axBinDeserializer se(buf);
			T i = kMin;
			for (;;) {
				T value;
				se.io(value);
				
				//AX_DUMP_VAR(value);
				AX_TEST_CHECK_SLIENT(value == i);

				if (i == kMax)
					break;
				i++;
			}
		}
	}

	template<class T>
	void test_int_fixed_sample() {
		const auto kMin = axNumLimit<T>::min();
		const auto kMax = axNumLimit<T>::max();

		const axInt kBits = ax_sizeof(T) * 8;

		axByteArray buf;
		{
			axBinSerializer se(buf);
			T min = kMin;
			T max = kMax;

			se.io_fixed(min);
			se.io_fixed(max);

			for (axInt i = 0; i < kBits; i++) {
				T v0 = T((T(1) << i));
				T v1 = T((T(1) << i) - 1);
				T v2 = T((T(1) << i) | (T(1) << (i/2)));
				T v3 = T((T(1) << i) | (T(1) << (i/3)));

				se.io_fixed(v0);
				se.io_fixed(v1);
				se.io_fixed(v2);
				se.io_fixed(v3);
			}
		}

		{
			axBinDeserializer se(buf);
			T min = 123;
			T max = 123;

			se.io_fixed(min); AX_TEST_CHECK_SLIENT(min == kMin);
			se.io_fixed(max); AX_TEST_CHECK_SLIENT(max == kMax);

			for (axInt i = 0; i < kBits; i++) {
				T k0 = T((T(1) << i));
				T k1 = T((T(1) << i) - 1);
				T k2 = T((T(1) << i) | (T(1) << (i/2)));
				T k3 = T((T(1) << i) | (T(1) << (i/3)));

				T v0 = 123;
				T v1 = 123;
				T v2 = 123;
				T v3 = 123;

				se.io_fixed(v0); AX_TEST_CHECK_SLIENT(v0 == k0);
				se.io_fixed(v1); AX_TEST_CHECK_SLIENT(v1 == k1);
				se.io_fixed(v2); AX_TEST_CHECK_SLIENT(v2 == k2);
				se.io_fixed(v3); AX_TEST_CHECK_SLIENT(v3 == k3);

//				AX_DUMP_VAR(v0, v1, v2, v3);
			}
		}
	}

	template<class T>
	void test_int_vary_sample() {
		const auto kMin = axNumLimit<T>::min();
		const auto kMax = axNumLimit<T>::max();

		const axInt kBits = ax_sizeof(T) * 8;

		axByteArray buf;
		{
			axBinSerializer se(buf);
			T min = kMin;
			T max = kMax;

			se.io(min);
			se.io(max);

			for (axInt i = 0; i < kBits; i++) {
				T v0 = T((T(1) << i));
				T v1 = T((T(1) << i) - 1);
				T v2 = T((T(1) << i) | (T(1) << (i/2)));
				T v3 = T((T(1) << i) | (T(1) << (i/3)));

				se.io(v0);
				se.io(v1);
				se.io(v2);
				se.io(v3);
			}
		}

		{
			axBinDeserializer se(buf);
			T min = 123;
			T max = 123;

			se.io(min); AX_TEST_CHECK_SLIENT(min == kMin);
			se.io(max); AX_TEST_CHECK_SLIENT(max == kMax);

			for (axInt i = 0; i < kBits; i++) {
				T v0 = T((T(1) << i));
				T v1 = T((T(1) << i) - 1);
				T v2 = T((T(1) << i) | (T(1) << (i/2)));
				T v3 = T((T(1) << i) | (T(1) << (i/3)));

				T t0 = 123;
				T t1 = 123;
				T t2 = 123;
				T t3 = 123;

				se.io(t0); AX_TEST_CHECK_SLIENT(v0 == t0);
				se.io(t1); AX_TEST_CHECK_SLIENT(v1 == t1);
				se.io(t2); AX_TEST_CHECK_SLIENT(v2 == t2);
				se.io(t3); AX_TEST_CHECK_SLIENT(v3 == t3);

				//AX_DUMP_VAR(v0, v1, v2, v3);
			}
		}
	}

	template<class T>
	void test_float_sample() {
		const auto kNan = ax_nan_<T>();
		const auto kInf = ax_inf_<T>();

		const axInt kBits = ax_sizeof(T) * 8;

		axByteArray buf;
		{
			axBinSerializer se(buf);
			T nan = kNan;
			T inf = kInf;

			se.io(nan);
			se.io(inf);

			for (axInt i = 0; i < kBits; i++) {
				T v0 = ax_pow(static_cast<T>(2),  static_cast<T>(i));
				T v1 = ax_pow(static_cast<T>(2), -static_cast<T>(i));
				T v2 = ax_pow(static_cast<T>(3),  static_cast<T>(i));
				T v3 = ax_pow(static_cast<T>(3), -static_cast<T>(i));

				se.io(v0);
				se.io(v1);
				se.io(v2);
				se.io(v3);
			}
		}

		{
			axBinDeserializer se(buf);
			T nan = 0;
			T inf = 0;

			se.io(nan); AX_TEST_CHECK_SLIENT(ax_is_nan(nan));
			se.io(inf); AX_TEST_CHECK_SLIENT(inf == kInf);

			for (axInt i = 0; i < kBits; i++) {
				T v0 = ax_pow(static_cast<T>(2),  static_cast<T>(i));
				T v1 = ax_pow(static_cast<T>(2), -static_cast<T>(i));
				T v2 = ax_pow(static_cast<T>(3),  static_cast<T>(i));
				T v3 = ax_pow(static_cast<T>(3), -static_cast<T>(i));

				T t0 = 123;
				T t1 = 123;
				T t2 = 123;
				T t3 = 123;

				se.io(t0); AX_TEST_CHECK_SLIENT(v0 == t0);
				se.io(t1); AX_TEST_CHECK_SLIENT(v1 == t1);
				se.io(t2); AX_TEST_CHECK_SLIENT(v2 == t2);
				se.io(t3); AX_TEST_CHECK_SLIENT(v3 == t3);

				//AX_DUMP_VAR(v0, v1, v2, v3);
			}
		}
	}
};

void test_axBinSerializer() {
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axUInt8 >());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axUInt16>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axUInt32>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axUInt64>());

	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axInt8 >());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axInt16>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axInt32>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_sample<axInt64>());

	//--------
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_all<axUInt8>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_all<axUInt16>());

	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_all<axInt8>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_fixed_all<axInt16>());

	//--------
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axUInt8 >());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axUInt16>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axUInt32>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axUInt64>());

	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axInt8 >());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axInt16>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axInt32>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_sample<axInt64>());

	//--------
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_all<axUInt8>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_all<axUInt16>());

	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_all<axInt8>());
	AX_TEST_CASE(Test_axBinSerializer, test_int_vary_all<axInt16>());

	//---- float ---
	AX_TEST_CASE(Test_axBinSerializer, test_float_sample<axFloat32>());
	AX_TEST_CASE(Test_axBinSerializer, test_float_sample<axFloat64>());

}
