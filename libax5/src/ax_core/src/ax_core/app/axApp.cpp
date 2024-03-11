#include <ax_core/app/axApp.h>
#include <ax_core/file/axPath.h>

#if AX_OS_WINDOWS
	#include <shellapi.h>
#endif

axApp* axApp_instance;

axApp* axApp::s_instance() {
	return axApp_instance;
}

axApp::axApp() {
	AX_ASSERT(axApp_instance == nullptr);
	axApp_instance = this;
}

axApp::~axApp() {
	AX_ASSERT(axApp_instance == this);
	axApp_instance = nullptr;
}

void axApp::setAppName(axStrView s) {
	_appName = s;
}

axStrView axApp::appName() {
	if (!_appName) {
		auto s = currentExecuteFilename();
		axPath::basename(s, false);
		_appName = s;
	}
	return _appName;
}

axStrView axApp::currentExecuteFilename() const {
	auto args = commandArguments();
	return args.size() ? args[0] : axStrView();
}

axSpan<axStrView> axApp::commandArguments() const { 
	auto* p = axAppArgments::getInstace();
	return p ? p->args() : axSpan<axStrView>();
}

int	axApp::_run() {
	try{
		return onRun();
	}catch(axError& err) {
		AX_LOG("Error: {?}", err);
		ax_flush_log();
		return -9999;
	}catch(std::exception& e) {
		AX_LOG("Error: std::exception {?}", e);
		ax_flush_log();
		return -9999;
	}catch(...) {
		AX_LOG("unknown exception");
		ax_flush_log();
		return -9999;
	}
}

//--------------

axAppArgments* axAppArgments_instance;

axAppArgments* axAppArgments::getInstace() {
	return axAppArgments_instance;
}

axAppArgments::axAppArgments(int argc, const char* argv[]) {
	AX_ASSERT(!axAppArgments_instance);
	axAppArgments_instance = this;

	#if AX_OS_WINDOWS
		int count = 0;
		auto* a = CommandLineToArgvW(GetCommandLineW(), &count);
		_argsStr.resize(count);
		for (axInt i=0; i<argc; i++) {
			_argsStr[i].setUtf_c_str(a[i]);
		}
	#elif AX_OS_MACOSX
		NSProcessInfo* info = [NSProcessInfo processInfo];
		NSArray* a = [info arguments];
		_argsStr.resize((axInt)a.count);
		for (size_t i=0; i<a.count; i++) {
			auto s = [[a objectAtIndex : i] UTF8String];
			_argsStr[(axInt)i].setUtf_c_str(s);
		}
	#else
		_argsStr.resize(argc);
		for (axInt i=0; i<argc; i++) {
			_argsStr[i].setUtf_c_str(argv[i]);
		}
	#endif

	_argsView.reserve(_argsStr.size());
	for (auto& e : _argsStr) {
		_argsView.append(e.view());
	}
}

axAppArgments::~axAppArgments() {
	AX_ASSERT(axAppArgments_instance == this);
	axAppArgments_instance = nullptr;
}

