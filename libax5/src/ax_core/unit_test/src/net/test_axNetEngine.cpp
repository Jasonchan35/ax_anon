#include <ax_core/net/engine/axNetEngine.h>
#include <ax_core/thread/_ALL_thread.h>

class Test_axNetEngine : public axUnitTestClass {
public:
	static axAtomicInt s_done;

	class MyServer : public axNetEngine {
	public:
		virtual void onConnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onErrorConnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onDisconnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onRecv(Socket* s) override {
			ax_log_func_name();
			axString_<1024> buf;
			s->recv(buf, 1024);
			AX_DUMP_VAR(buf);

			receivedCount += buf.size();
			if (receivedCount >= 10) {
				s_done = 1;
			}
		}
		virtual void onError(Socket* s, axError& error) override {
			ax_log_func_name();
		}

		axInt receivedCount = 0;
	};

	class MyClient : public axNetEngine {
	public:
		virtual void onConnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onErrorConnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onDisconnect(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onRecv(Socket* s) override {
			ax_log_func_name();
		}
		virtual void onError(Socket* s, axError& error) override {
			ax_log_func_name();
		}
	};

	void test_case1() {
		const static axUInt16 port = 30001;

		MyServer server;
		AX_LOG("server: listen...");
		server.listen("0.0.0.0", port);

		axThread thread;
		thread.start("test_NewEngine", [](){
			MyClient client;
			AX_LOG("client: connect");
			client.connect("localhost", port);

			AX_LOG("client: send");
			client.sendStringToAll("Hello");
			client.sendStringToAll("World");

			while (!s_done) {
				client.update(10);
			}
		});

		while (!s_done) {
			server.update(10);
		}
	}
};

axAtomicInt Test_axNetEngine::s_done;

void test_axNetEngine() {
	AX_TEST_CASE(Test_axNetEngine, test_case1());
}
