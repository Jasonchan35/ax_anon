#include "axFileCmd.h"

void axFileCmd::showHelp() {
	AX_LOG(	"==== axFileCmd Help: ===="
			"  axFileCmd -touch <filename>\n"
			"  axFileCmd -dos2unix <filename>\n"
			"\n");	
}

int axFileCmd::onRun() {
	auto args = commandArguments();

	for (axInt i=1; i<args.size(); i++) {
		auto& a = args[i];
		if (a == "-touch") {
			i++;
			if (i >= args.size()) {
				throw axError_Undefined(AX_LOC);
			}

			axFile::touch(args[i]);
			continue;
		}

		if (a == "-platform_os") {
			i++;
			auto name = axSystemInfo::getOSName();
			ax_console_write(name);
			ax_console_write("\n");
			continue;
		}

		if (a == "-dos2unix") {
			i++;
			if (i >= args.size()) {
				throw axError_Undefined(AX_LOC);
			}
			axString8_<64 * 1024> text;
			axFile::readUtf8(args[i], text);
			if (text.replaceAll("\r\n", "\n", false) > 0) {
				axFile::writeUtf8(args[i], text);
			}
			continue;
		}

		AX_LOG("Unknown argument {?}", a);
		showHelp();
		return -1;
	}

	return 0;
}
