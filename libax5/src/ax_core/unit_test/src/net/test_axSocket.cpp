#include <ax_core/net/axSocket.h>

class Test_axSocket : public axUnitTestClass {
public:
	void test_resolveMultiple() {
		axArray<axSockAddr, 16> addrs;
		axSockAddr::s_tryResolveMutliple(addrs, "localhost", 0, axSockAddrFamily::Unknown);
		AX_DUMP_VAR(addrs);
	}

	void test_resolveIPv4() {
		axIPv4 ip;
		ip.resolve("localhost");
		AX_DUMP_VAR(ip);
		AX_TEST_CHECK(ip == axIPv4(127,0,0,1));
	}

	void test_resolveIPv6() {
		axIPv6 ip;
		ip.resolve("localhost");
		AX_DUMP_VAR(ip);
		AX_TEST_CHECK(ip == axIPv6(	0,0,0,0,
									0,0,0,0,
									0,0,0,0,
									0,0,0,1));
	}

	void test_TCP() {
		static const axUInt16 port = 12345;
		static const axInt N = 20 * 1000 * 1000;
		axByteArray testData;
		testData.resize(N);
		{
			axInt i = 0;
			for (auto& e : testData) {
				e = static_cast<axByte>(i);
				i++;
			}
		}

		axTcpSocket s;
		s.bindAndListen("localhost", port);

		axThread thread;
		thread.start("test_axSocket", [&testData]() {
			axTcpSocket c;
			c.connect("localhost", port);
			c.send(testData);
		});

		axTcpSocket a;
		s.accept(a);
		axByteArray buf;

		for (;;) {
			ax_sleep(10);
			auto avail = a.availableBytes();
			//AX_DUMP_VAR(avail);

			if (!avail) {
				continue;
			}

			auto done = a.fillBuffer(buf, N);
			//AX_DUMP_VAR(buf.size());
			if (done)
				break;
		}

		{
			axInt i = 0;
			for (auto& e : buf) {
				if (e != static_cast<axByte>(i)) {
					AX_ASSERT(false);
				}
				i++;
			}
		}

//		AX_DUMP_HEX(buf);
		AX_TEST_CHECK(buf == testData);
	}
};

void test_axSocket() {
	AX_TEST_CASE(Test_axSocket, test_resolveMultiple());
	AX_TEST_CASE(Test_axSocket, test_resolveIPv4());
	AX_TEST_CASE(Test_axSocket, test_resolveIPv6());

	AX_TEST_CASE(Test_axSocket, test_TCP());
}
